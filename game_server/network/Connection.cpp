/// @author    Chnossos
/// @date      Created on 2026-02-22

#include "Connection.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/network/Packet.hpp>

// Third-party includes
#include <boost/asio/read.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <algorithm>
#include <iostream>
#include <vector>

using Network::Connection;

using EncryptionKey = std::array<byte, sizeof(u64)>;
constexpr EncryptionKey gEncryptionKey {{0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c, 0x54, 0x87}};

struct Connection::ConnectionImpl
{
    boost::asio::ip::tcp::socket socket;
    std::vector<byte> readBuffer;
    EncryptionKey encryptionKey{}, decryptionKey{};

    explicit ConnectionImpl(boost::asio::ip::tcp::socket && socket);

    void encrypt(std::span<byte> data);
    void decrypt(std::span<byte> data);
};

Connection::ConnectionImpl::ConnectionImpl(boost::asio::ip::tcp::socket && socket)
    : socket(std::move(socket))
{
    readBuffer.resize(sizeof(PacketHeader));
}

void Connection::ConnectionImpl::encrypt(std::span<byte> data)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i];
        data[i] = static_cast<byte>(tmp2 ^ encryptionKey[i & 7] ^ tmp1);
        tmp1    = data[i];
    }

    tmp1  = encryptionKey[0] << 0x00 & 0xFF;
    tmp1 |= encryptionKey[1] << 0x08 & 0xFF00;
    tmp1 |= encryptionKey[2] << 0x10 & 0xFF0000;
    tmp1 |= encryptionKey[3] << 0x18 & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    encryptionKey[0] = static_cast<byte>(tmp1 >> 0x00 & 0xFF);
    encryptionKey[1] = static_cast<byte>(tmp1 >> 0x08 & 0xFF);
    encryptionKey[2] = static_cast<byte>(tmp1 >> 0x10 & 0xFF);
    encryptionKey[3] = static_cast<byte>(tmp1 >> 0x18 & 0xFF);
}

void Connection::ConnectionImpl::decrypt(std::span<byte> data)
{
    L2CPP_B_ASSERT(data.size() <= std::numeric_limits<u16>::max(), "Data size ({}) > UINT16_MAX", data.size());

    u32 tmp1 = 0, tmp2 = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        tmp2    = data[i];
        data[i] = static_cast<byte>(tmp2 ^ decryptionKey[i & 7] ^ tmp1);
        tmp1    = tmp2;
    }

    tmp1  = decryptionKey[0] << 0x00 & 0xFF;
    tmp1 |= decryptionKey[1] << 0x08 & 0xFF00;
    tmp1 |= decryptionKey[2] << 0x10 & 0xFF0000;
    tmp1 |= decryptionKey[3] << 0x18 & 0xFF000000;

    tmp1 += static_cast<u16>(data.size());

    decryptionKey[0] = static_cast<byte>(tmp1 >> 0x00 & 0xFF);
    decryptionKey[1] = static_cast<byte>(tmp1 >> 0x08 & 0xFF);
    decryptionKey[2] = static_cast<byte>(tmp1 >> 0x10 & 0xFF);
    decryptionKey[3] = static_cast<byte>(tmp1 >> 0x18 & 0xFF);
}

// ---------------------------------------------------------------------------------------------------------------------

Connection::Connection(boost::asio::ip::tcp::socket && socket)
    : _impl(std::move(socket))
{}

Connection::~Connection() = default;

void Connection::read()
{
    // Reset buffer
    std::ranges::fill(_impl->readBuffer, 0);

    // Read packet size
    boost::system::error_code ec;
    boost::asio::read(_impl->socket, boost::asio::buffer(_impl->readBuffer.data(), sizeof(PacketHeader)), ec);
    if (ec.value() == boost::asio::error::eof)
        L2CPP_THROW("Client disconnected");

    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    auto const size = *reinterpret_cast<PacketHeader *>(_impl->readBuffer.data());
    L2CPP_B_ASSERT(size > sizeof(size), "Packet announced without any opCode");

    if (_impl->readBuffer.size() < size)
        _impl->readBuffer.resize(size);

    auto request  = _impl->readBuffer.data() + sizeof(size);
    auto bodySize = size - sizeof(size);

    boost::asio::read(_impl->socket, boost::asio::buffer(request, bodySize), ec);
    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    if (std::ranges::any_of(_impl->decryptionKey, [] (auto const c) { return c != 0x00; })) [[likely]]
        _impl->decrypt({request, bodySize});
    else
        _impl->decryptionKey = gEncryptionKey;
}

void Connection::send(l2cpp::Network::Packet & p)
{
    L2CPP_B_ASSERT(p.opCode().has_value(), "Trying to send a packet without any opCode");
    auto const opCode = p.opCode().value();

    p.writeSize();

#ifndef NDEBUG
    SPDLOG_INFO("sent: 0x{:0{}x} ({} bytes)", opCode, opCode > 0xff ? 4 : 2, p.size());
    std::cout << l2cpp::hexdump(p.body().data(), p.bodySize()) << std::endl;
#endif

    if (std::ranges::any_of(_impl->encryptionKey, [] (auto const c) { return c != 0x00; })) [[likely]]
        _impl->encrypt(p.body());
    else
        _impl->encryptionKey = gEncryptionKey;

    _impl->socket.send(boost::asio::buffer(p.buffer()));

#ifdef NDEBUG
    SPDLOG_INFO("sent: 0x{:0{}x} ({} bytes)", opCode, opCode > 0xff ? 4 : 2, p.size());
#endif
}

void Connection::send(l2cpp::Network::Packet && p)
{
    send(p);
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
    return gEncryptionKey;
}
