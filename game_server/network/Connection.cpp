/// @author    Chnossos
/// @date      Created on 2026-02-22

#include "Connection.hpp"

// Project includes
#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/network/Packet.hpp>

// Third-party includes
#include <boost/asio/read.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <algorithm>
#include <iostream>
#include <vector>

using Network::Connection;

struct Connection::ConnectionImpl
{
    boost::asio::ip::tcp::socket socket;
    std::vector<byte> readBuffer;
    std::array<byte, sizeof(u64)> encryptionKey, decryptionKey;

    explicit ConnectionImpl(boost::asio::ip::tcp::socket && socket);

    void encrypt(std::span<byte> data);
    void decrypt(std::span<byte> data);
};

Connection::ConnectionImpl::ConnectionImpl(boost::asio::ip::tcp::socket && socket)
    : socket(std::move(socket))
    , encryptionKey{{0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c, 0x54, 0x87}}
    , decryptionKey(encryptionKey)
{
    readBuffer.resize(sizeof(u16));
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
    : _impl(std::make_unique<ConnectionImpl>(std::move(socket)))
{}

Connection::~Connection() = default;

void Connection::read(bool const decrypt)
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
    L2CPP_B_ASSERT(size, "Packet announced without any opCode");

    if (_impl->readBuffer.size() < size)
        _impl->readBuffer.resize(size);

    auto request  = _impl->readBuffer.data() + sizeof(size);
    auto bodySize = size - sizeof(size);

    boost::asio::read(_impl->socket, boost::asio::buffer(request, bodySize), ec);
    L2CPP_BC_ASSERT(!ec, ec.value(), "read error: {}", ec.message());

    if (decrypt)
        _impl->decrypt({request, bodySize});
}

void Connection::send(l2cpp::Network::Packet & p, bool const encrypt)
{
    L2CPP_B_ASSERT(p.opCode().has_value(), "Trying to send a packet without any opCode");

    p.writeSize();

    SPDLOG_INFO("sent: 0x{:02x} ({} bytes)", p.opCode().value(), p.size());
    std::cout << l2cpp::hexdump(p.buffer().data(), p.buffer().size()) << std::endl;

    if (encrypt)
        _impl->encrypt(p.body());

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
