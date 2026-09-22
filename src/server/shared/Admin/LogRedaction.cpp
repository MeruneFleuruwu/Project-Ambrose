/*
 * Project Ambrose by Imjustchico
 * Names the secret settings, masks a connection string's password segment and a token's whole value, and scrubs any text that quotes a secret setting's value after its key.
 */

#include "LogRedaction.h"
#include "StringUtil.h"

#include <cctype>
#include <string>

namespace
{
    bool EndsWith(std::string_view text, std::string_view suffix) noexcept
    {
        return text.size() >= suffix.size() && text.substr(text.size() - suffix.size()) == suffix;
    }

    std::string MaskConnectionString(std::string_view value)
    {
        std::string masked;
        std::size_t field = 0;
        std::size_t start = 0;
        while (true)
        {
            std::size_t const end = value.find(';', start);
            std::string_view const part = value.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start);
            if (field == 3)
                masked.append(LogRedaction::Mask);
            else
                masked.append(part);
            if (end == std::string_view::npos)
                break;
            masked.push_back(';');
            start = end + 1;
            ++field;
        }
        return masked;
    }

    bool IsKeyCharacter(char character) noexcept
    {
        return std::isalnum(static_cast<unsigned char>(character)) || character == '.' || character == '_';
    }
}

bool LogRedaction::IsSecretSetting(std::string_view key) noexcept
{
    std::string const lowered = Ambrose::ToLower(std::string(key));
    return lowered == "admin.token" || EndsWith(lowered, "databaseinfo");
}

std::string LogRedaction::RedactSettingValue(std::string_view key, std::string_view value)
{
    if (!IsSecretSetting(key))
        return std::string(value);
    if (EndsWith(Ambrose::ToLower(std::string(key)), "databaseinfo"))
        return MaskConnectionString(value);
    return std::string(Mask);
}

std::string LogRedaction::DescribeSettingChange(std::string_view key, std::string_view value, std::string_view source)
{
    std::string text = "Setting ";
    text.append(key);
    text.append(" changed to ");
    text.append(RedactSettingValue(key, value));
    if (!source.empty())
    {
        text.append(" from ");
        text.append(source);
    }
    return text;
}

std::string LogRedaction::Redact(std::string_view text)
{
    std::string out;
    out.reserve(text.size());
    std::size_t position = 0;
    while (position < text.size())
    {
        std::size_t keyStart = position;
        while (keyStart < text.size() && !IsKeyCharacter(text[keyStart]))
            ++keyStart;
        std::size_t keyEnd = keyStart;
        while (keyEnd < text.size() && IsKeyCharacter(text[keyEnd]))
            ++keyEnd;
        out.append(text.substr(position, keyEnd - position));
        position = keyEnd;
        if (keyStart == keyEnd || !IsSecretSetting(text.substr(keyStart, keyEnd - keyStart)))
            continue;
        std::size_t cursor = keyEnd;
        while (cursor < text.size() && (text[cursor] == ' ' || text[cursor] == '\t'))
            ++cursor;
        if (cursor >= text.size() || (text[cursor] != '=' && text[cursor] != ':'))
            continue;
        ++cursor;
        while (cursor < text.size() && (text[cursor] == ' ' || text[cursor] == '\t'))
            ++cursor;
        bool const quoted = cursor < text.size() && text[cursor] == '"';
        if (quoted)
            ++cursor;
        std::size_t valueEnd = cursor;
        if (quoted)
        {
            while (valueEnd < text.size() && text[valueEnd] != '"')
                ++valueEnd;
        }
        else
        {
            while (valueEnd < text.size() && text[valueEnd] != ' ' && text[valueEnd] != ',' && text[valueEnd] != '\n' && text[valueEnd] != '\r')
                ++valueEnd;
        }
        out.append(text.substr(keyEnd, cursor - keyEnd));
        out.append(RedactSettingValue(text.substr(keyStart, keyEnd - keyStart), text.substr(cursor, valueEnd - cursor)));
        position = valueEnd;
    }
    return out;
}
