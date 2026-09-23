/*
 * Project Ambrose by Imjustchico
 * Implements the bounded binary type-registry cache envelope with a format version, client revision and content hash.
 */

#include "TypeRegistryBinary.h"

#include "Hex.h"
#include "SHA256.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <limits>
#include <span>

namespace
{
    constexpr std::array<char, 8> Magic{ 'A', 'M', 'B', 'T', 'R', 'E', 'G', '1' };
    constexpr uint32 Version = 1;
    constexpr std::uintmax_t MaxPayloadBytes = std::uintmax_t{ 512 } << 20;
    constexpr uint32 HashBytes = 64;

    void AppendU32(std::string& bytes, uint32 value)
    {
        for (int shift = 0; shift != 32; shift += 8)
            bytes.push_back(static_cast<char>((value >> shift) & 0xFFu));
    }

    void AppendU64(std::string& bytes, uint64 value)
    {
        for (int shift = 0; shift != 64; shift += 8)
            bytes.push_back(static_cast<char>((value >> shift) & 0xFFu));
    }

    bool ReadU32(std::string_view bytes, std::size_t& offset, uint32& value)
    {
        if (bytes.size() - offset < 4)
            return false;
        value = uint32(static_cast<unsigned char>(bytes[offset])) |
            (uint32(static_cast<unsigned char>(bytes[offset + 1])) << 8) |
            (uint32(static_cast<unsigned char>(bytes[offset + 2])) << 16) |
            (uint32(static_cast<unsigned char>(bytes[offset + 3])) << 24);
        offset += 4;
        return true;
    }

    bool ReadU64(std::string_view bytes, std::size_t& offset, uint64& value)
    {
        if (bytes.size() - offset < 8)
            return false;
        value = 0;
        for (int shift = 0; shift != 64; shift += 8)
            value |= uint64(static_cast<unsigned char>(bytes[offset++])) << shift;
        return true;
    }

    bool ReadString(std::string_view bytes, std::size_t& offset, uint64 length, std::string& value)
    {
        if (length > bytes.size() - offset)
            return false;
        value.assign(bytes.substr(offset, static_cast<std::size_t>(length)));
        offset += static_cast<std::size_t>(length);
        return true;
    }
}

bool TypeRegistryBinary::Write(std::filesystem::path const& output, std::string_view json, std::string_view revision, std::string& error)
{
    if (json.size() > MaxPayloadBytes || revision.size() > std::numeric_limits<uint32>::max())
    {
        error = "the type dump or revision is too large for the binary cache";
        return false;
    }

    std::string const hash = Hex::Encode(SHA256::GetDigestOf(std::span<uint8 const>(reinterpret_cast<uint8 const*>(json.data()), json.size())));
    std::string bytes;
    bytes.reserve(Magic.size() + 4 + 4 + 8 + 4 + revision.size() + hash.size() + json.size());
    bytes.append(Magic.data(), Magic.size());
    AppendU32(bytes, Version);
    AppendU32(bytes, static_cast<uint32>(revision.size()));
    AppendU64(bytes, json.size());
    AppendU32(bytes, HashBytes);
    bytes.append(revision);
    bytes.append(hash);
    bytes.append(json);

    std::ofstream stream(output, std::ios::binary | std::ios::trunc);
    if (!stream || !stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size())))
    {
        error = "cannot write " + output.string();
        return false;
    }
    return true;
}

bool TypeRegistryBinary::Read(std::filesystem::path const& input, std::string_view expectedRevision, std::string& json, std::string& revision, std::string& error)
{
    std::error_code fileError;
    std::uintmax_t const size = std::filesystem::file_size(input, fileError);
    if (fileError)
    {
        error = "cannot stat " + input.string() + ": " + fileError.message();
        return false;
    }
    if (size > MaxPayloadBytes + 128)
    {
        error = "the binary type-registry cache is too large";
        return false;
    }
    std::string bytes(static_cast<std::size_t>(size), '\0');
    std::ifstream stream(input, std::ios::binary);
    if (!stream || !stream.read(bytes.data(), static_cast<std::streamsize>(bytes.size())))
    {
        error = "cannot read " + input.string();
        return false;
    }

    std::size_t offset = 0;
    if (bytes.size() < Magic.size() || !std::equal(Magic.begin(), Magic.end(), bytes.begin()))
    {
        error = "the binary type-registry cache has an invalid magic";
        return false;
    }
    offset += Magic.size();
    uint32 version = 0;
    uint32 revisionLength = 0;
    uint64 payloadLength = 0;
    uint32 hashLength = 0;
    if (!ReadU32(bytes, offset, version) || !ReadU32(bytes, offset, revisionLength) || !ReadU64(bytes, offset, payloadLength) || !ReadU32(bytes, offset, hashLength) ||
        version != Version || hashLength != HashBytes)
    {
        error = "the binary type-registry cache has an unsupported header";
        return false;
    }
    std::string storedHash;
    if (!ReadString(bytes, offset, revisionLength, revision) || !ReadString(bytes, offset, hashLength, storedHash) || !ReadString(bytes, offset, payloadLength, json) ||
        offset != bytes.size())
    {
        error = "the binary type-registry cache is truncated";
        return false;
    }
    if (!expectedRevision.empty() && revision != expectedRevision)
    {
        error = "the binary type-registry cache is for revision " + revision + ", expected " + std::string(expectedRevision);
        return false;
    }
    std::string const actualHash = Hex::Encode(SHA256::GetDigestOf(std::span<uint8 const>(reinterpret_cast<uint8 const*>(json.data()), json.size())));
    if (storedHash != actualHash)
    {
        error = "the binary type-registry cache SHA-256 does not match its payload";
        return false;
    }
    return true;
}
