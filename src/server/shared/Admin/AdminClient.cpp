/*
 * Project Ambrose by Imjustchico
 * Sends one request over standalone Asio with Connection: close and waits for the listener to close, all inside one deadline that closes the socket when it passes; a listener bound to every address is reached on loopback, an IPv6 host is bracketed, an answer larger than MaxResponseBytes is refused, and the body is read by its Content-Length or its chunks.
 */

#include "AdminClient.h"
#include "StringUtil.h"

#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

#include <fmt/format.h>

#include <array>
#include <charconv>
#include <functional>
#include <system_error>
#include <utility>

namespace
{
    std::optional<std::string> HeaderValue(std::string_view head, std::string_view name)
    {
        std::size_t position = head.find("\r\n");
        while (position != std::string_view::npos && position + 2 < head.size())
        {
            std::size_t const start = position + 2;
            std::size_t const end = head.find("\r\n", start);
            std::string_view const line = head.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start);
            std::size_t const colon = line.find(':');
            if (colon != std::string_view::npos && Ambrose::EqualsIgnoreCase(Ambrose::Trim(line.substr(0, colon)), name))
                return std::string(Ambrose::Trim(line.substr(colon + 1)));
            position = end;
        }
        return std::nullopt;
    }

    bool DecodeChunks(std::string_view body, std::string& decoded, std::string& error)
    {
        decoded.clear();
        while (true)
        {
            std::size_t const lineEnd = body.find("\r\n");
            if (lineEnd == std::string_view::npos)
            {
                error = "a chunk size line never ends";
                return false;
            }
            std::string_view sizeText = body.substr(0, lineEnd);
            if (std::size_t const extension = sizeText.find(';'); extension != std::string_view::npos)
                sizeText = sizeText.substr(0, extension);
            sizeText = Ambrose::Trim(sizeText);
            std::size_t size = 0;
            auto const [end, parsed] = std::from_chars(sizeText.data(), sizeText.data() + sizeText.size(), size, 16);
            if (sizeText.empty() || parsed != std::errc() || end != sizeText.data() + sizeText.size())
            {
                error = "a chunk size is not hexadecimal";
                return false;
            }
            body.remove_prefix(lineEnd + 2);
            if (size == 0)
                return true;
            if (body.size() < size + 2 || body.substr(size, 2) != "\r\n")
            {
                error = "a chunk is shorter than its size";
                return false;
            }
            decoded.append(body.substr(0, size));
            if (decoded.size() > AdminClient::MaxResponseBytes)
            {
                error = "the answer is larger than the client takes";
                return false;
            }
            body.remove_prefix(size + 2);
        }
    }
}

AdminClient::AdminClient(std::string host, uint16 port, std::string token) : _host(std::move(host)), _port(port), _token(std::move(token))
{
}

std::string AdminClient::ConnectHost(std::string_view bindIp)
{
    std::string_view const trimmed = Ambrose::Trim(bindIp);
    if (trimmed.empty() || trimmed == "0.0.0.0")
        return "127.0.0.1";
    if (trimmed == "::" || trimmed == "[::]")
        return "::1";
    if (trimmed.size() > 2 && trimmed.front() == '[' && trimmed.back() == ']')
        return std::string(trimmed.substr(1, trimmed.size() - 2));
    return std::string(trimmed);
}

std::optional<AdminClientResponse> AdminClient::Parse(std::string_view raw, std::string& error)
{
    std::size_t const headEnd = raw.find("\r\n\r\n");
    if (headEnd == std::string_view::npos)
    {
        error = "the answer has no complete header";
        return std::nullopt;
    }
    std::string_view const head = raw.substr(0, headEnd);
    std::string_view const statusLine = head.substr(0, head.find("\r\n"));
    if (!statusLine.starts_with("HTTP/1."))
    {
        error = "the answer is not HTTP/1.x";
        return std::nullopt;
    }
    std::size_t const space = statusLine.find(' ');
    int status = 0;
    std::string_view const code = space == std::string_view::npos ? std::string_view() : statusLine.substr(space + 1, 3);
    auto const [codeEnd, codeParsed] = std::from_chars(code.data(), code.data() + code.size(), status);
    if (code.size() != 3 || codeParsed != std::errc() || codeEnd != code.data() + code.size() || status < 100 || status > 599)
    {
        error = "the answer has no status code";
        return std::nullopt;
    }
    AdminClientResponse response;
    response.Answered = true;
    response.Status = status;
    response.ContentType = HeaderValue(head, "Content-Type").value_or(std::string());
    response.RequestId = HeaderValue(head, "X-Request-Id").value_or(std::string());
    std::string_view const body = raw.substr(headEnd + 4);
    std::optional<std::string> const encoding = HeaderValue(head, "Transfer-Encoding");
    if (encoding && Ambrose::EqualsIgnoreCase(*encoding, "chunked"))
    {
        if (!DecodeChunks(body, response.Body, error))
            return std::nullopt;
        return response;
    }
    if (std::optional<std::string> const length = HeaderValue(head, "Content-Length"))
    {
        std::size_t size = 0;
        auto const [end, parsed] = std::from_chars(length->data(), length->data() + length->size(), size);
        if (length->empty() || parsed != std::errc() || end != length->data() + length->size())
        {
            error = "the answer's Content-Length is not a number";
            return std::nullopt;
        }
        if (body.size() < size)
        {
            error = fmt::format("the answer ended after {} of its {} bytes", body.size(), size);
            return std::nullopt;
        }
        response.Body = std::string(body.substr(0, size));
        return response;
    }
    response.Body = std::string(body);
    return response;
}

AdminClientResponse AdminClient::Send(AdminClientRequest const& request, std::chrono::milliseconds timeout) const
{
    AdminClientResponse response;
    std::error_code addressError;
    asio::ip::address const address = asio::ip::make_address(_host, addressError);
    if (addressError)
    {
        response.Error = fmt::format("{} is not an IP address", _host);
        return response;
    }
    std::string const host = address.is_v6() ? fmt::format("[{}]:{}", _host, _port) : fmt::format("{}:{}", _host, _port);
    std::string wire = fmt::format("{} {} HTTP/1.1\r\nHost: {}\r\nConnection: close\r\nAccept: application/json\r\n", Ambrose::ToUpper(request.Method), request.Path, host);
    if (!_token.empty())
        wire += fmt::format("Authorization: Bearer {}\r\n", _token);
    if (!request.RequestId.empty())
        wire += fmt::format("X-Request-Id: {}\r\n", request.RequestId);
    if (!request.Body.empty() || request.Method != "GET")
        wire += fmt::format("Content-Type: {}\r\nContent-Length: {}\r\n", request.ContentType, request.Body.size());
    wire += "\r\n";
    wire += request.Body;

    asio::io_context context;
    asio::ip::tcp::socket socket(context);
    std::string raw;
    std::array<char, 16384> buffer{};
    std::string failure;
    bool finished = false;
    std::function<void()> readMore;
    readMore = [&]
    {
        socket.async_read_some(asio::buffer(buffer), [&](std::error_code const& error, std::size_t bytes)
        {
            raw.append(buffer.data(), bytes);
            if (raw.size() > MaxResponseBytes + 65536)
            {
                failure = "the answer is larger than the client takes";
                finished = true;
                return;
            }
            if (!error)
            {
                readMore();
                return;
            }
            if (error != asio::error::eof && error != asio::error::connection_reset)
                failure = fmt::format("the answer could not be read: {}", error.message());
            finished = true;
        });
    };
    socket.async_connect(asio::ip::tcp::endpoint(address, _port), [&](std::error_code const& error)
    {
        if (error)
        {
            failure = fmt::format("no admin API answers on {}: {}", host, error.message());
            finished = true;
            return;
        }
        asio::async_write(socket, asio::buffer(wire), [&](std::error_code const& written, std::size_t)
        {
            if (written)
            {
                failure = fmt::format("the request could not be sent: {}", written.message());
                finished = true;
                return;
            }
            readMore();
        });
    });
    context.run_for(timeout);
    if (!finished)
    {
        std::error_code ignored;
        socket.close(ignored);
        context.restart();
        context.poll();
        response.Error = fmt::format("the admin API on {} gave no answer within {} ms", host, timeout.count());
        return response;
    }
    if (!failure.empty() && raw.find("\r\n\r\n") == std::string::npos)
    {
        response.Error = failure;
        return response;
    }
    std::string parseError;
    std::optional<AdminClientResponse> parsed = Parse(raw, parseError);
    if (!parsed)
    {
        response.Error = fmt::format("the admin API on {} gave an answer that could not be read: {}", host, parseError);
        return response;
    }
    return std::move(*parsed);
}
