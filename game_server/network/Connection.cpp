/// @author    Chnossos
/// @date      Created on 2026-02-22

#include "Connection.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>

// Third-party includes
#include <boost/asio/read.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <algorithm>
#include <iostream>
#include <vector>

static void encrypt(std::span<byte> data, std::array<byte, sizeof(u64)> & key)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ key[i & 7] & 0xFF ^ tmp1);
        tmp1    = data[i];
    }

    tmp1  = key[0] << 0x00 & 0xFF;
    tmp1 |= key[1] << 0x08 & 0xFF00;
    tmp1 |= key[2] << 0x10 & 0xFF0000;
    tmp1 |= key[3] << 0x18 & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[0] = static_cast<byte>(tmp1 >> 0x00 & 0xFF);
    key[1] = static_cast<byte>(tmp1 >> 0x08 & 0xFF);
    key[2] = static_cast<byte>(tmp1 >> 0x10 & 0xFF);
    key[3] = static_cast<byte>(tmp1 >> 0x18 & 0xFF);
}

static void decrypt(std::span<byte> data, std::array<byte, sizeof(u64)> & key)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i] & 0xFF;
        data[i] = static_cast<byte>(tmp2 ^ key[i & 7] & 0xFF ^ tmp1);
        tmp1    = tmp2;
    }

    tmp1  = key[0] << 0x00 & 0xFF;
    tmp1 |= key[1] << 0x08 & 0xFF00;
    tmp1 |= key[2] << 0x10 & 0xFF0000;
    tmp1 |= key[3] << 0x18 & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    key[0] = static_cast<byte>(tmp1 >> 0x00 & 0xFF);
    key[1] = static_cast<byte>(tmp1 >> 0x08 & 0xFF);
    key[2] = static_cast<byte>(tmp1 >> 0x10 & 0xFF);
    key[3] = static_cast<byte>(tmp1 >> 0x18 & 0xFF);
}

struct Connection::ConnectionImpl
{
    boost::asio::ip::tcp::socket       socket;
    std::vector<byte> readBuffer;
    std::array<byte, sizeof(u64)> encryptionKey, decryptionKey;

    explicit ConnectionImpl(boost::asio::ip::tcp::socket && socket)
        : socket(std::move(socket))
        , encryptionKey{{0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c, 0x54, 0x87}}
        , decryptionKey(encryptionKey)
    {
        readBuffer.resize(sizeof(u16));
    }
};

Connection::Connection(boost::asio::ip::tcp::socket && socket)
    : _impl(std::make_unique<ConnectionImpl>(std::move(socket)))
{}

Connection::~Connection() = default;

void Connection::read(bool const needDecryption)
{
    // Reset buffer
    std::ranges::fill(_impl->readBuffer, 0);

    // Read packet size
    boost::system::error_code ec;
    boost::asio::read(_impl->socket, boost::asio::buffer(_impl->readBuffer.data(), sizeof(u16)), ec);
    if (ec.value() == boost::asio::error::eof)
        L2CPP_THROW("Client disconnected");

    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    auto const size = *reinterpret_cast<u16 *>(_impl->readBuffer.data());
    L2CPP_B_ASSERT(size, "Packet without body");
    SPDLOG_TRACE("Incoming packet of size '{}'", size);

    if (_impl->readBuffer.size() < size)
    {
        auto const oldSize = _impl->readBuffer.size();
        _impl->readBuffer.resize(size);
        SPDLOG_TRACE("_impl->readBuffer resized from '{}' to '{}'", oldSize, _impl->readBuffer.size());
    }

    auto request  = _impl->readBuffer.data() + sizeof(size);
    auto bodySize = size - sizeof(size);

    SPDLOG_TRACE("Attempt to read next {} bytes", bodySize);
    boost::asio::read(_impl->socket, boost::asio::buffer(request, bodySize), ec);
    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    if (needDecryption)
        decrypt({request, bodySize}, _impl->decryptionKey);

    // Read packet type
    auto const type = request[0];
    SPDLOG_INFO("recv: 0x{:02x} ({} bytes)", type, size);
}

void Connection::send(Packet & p, bool const needEncryption)
{
    p.writeSize();

    SPDLOG_DEBUG("About to send packet 0x{:02x} ({} bytes)", p.opCode().value_or(0xFF), p.size());
    std::cout << l2cpp::hexdump(p.buffer().data(), p.buffer().size()) << std::endl;

    if (needEncryption)
        encrypt(p.body(), _impl->encryptionKey);

    _impl->socket.send(boost::asio::buffer(p.buffer()));
}

void Connection::close()
{
    _impl->socket.close();
}

bool Connection::isAlive() const
{
    return _impl->socket.is_open();
}

auto Connection::readBuffer() const -> std::span<byte const>
{
    return _impl->readBuffer;
}

auto Connection::encryptionKey() const -> std::span<byte const>
{
    return _impl->encryptionKey;
}
