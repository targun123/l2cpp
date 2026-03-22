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
#include <boost/asio/write.hpp>

using Network::Connection;

using EncryptionKey = std::array<byte, sizeof(u64)>;
constexpr EncryptionKey gEncryptionKey {{0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c, 0x54, 0x87}};

struct Connection::ConnectionImpl
{
    boost::asio::ip::tcp::socket socket;
    std::vector<byte> readBuffer;
    EncryptionKey encryptionKey{}, decryptionKey{};
    PacketReceivedHandler packetHandler;

    explicit ConnectionImpl(boost::asio::ip::tcp::socket && socket);

    void encrypt(std::span<byte> data);
    void decrypt(std::span<byte> data);
    void close();

    void onSizeRead(boost::system::error_code const & ec, std::size_t bytes_transferred);
    void onBodyRead(boost::system::error_code const & ec, std::size_t bytes_transferred);
    bool handleReadError(boost::system::error_code const & ec, std::string_view source);
};

template class Pimpl<Connection::ConnectionImpl>;

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

void Connection::ConnectionImpl::close()
{
    if (socket.is_open())
    {
        boost::system::error_code ec;
        if (socket.shutdown(socket.shutdown_both, ec))
            SPDLOG_ERROR("shutdown error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

        ec.clear();
        if (socket.close(ec))
            SPDLOG_ERROR("close error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());
    }
}

void Connection::ConnectionImpl::onSizeRead(boost::system::error_code const & ec, std::size_t)
{
    if (ec.value() == boost::asio::error::operation_aborted || !handleReadError(ec, "size"))
        return;

    auto const size = *reinterpret_cast<PacketHeader const *>(readBuffer.data());
    readBuffer.resize(size);

    boost::asio::async_read(socket, boost::asio::buffer(readBuffer.data() + sizeof(size), size - sizeof(size)),
                            [this] (auto const & ec, auto sz) { onBodyRead(ec, sz); });
}

void Connection::ConnectionImpl::onBodyRead(boost::system::error_code const & ec, std::size_t packetBodySize)
{
    if (ec.value() == boost::asio::error::operation_aborted || !handleReadError(ec, "body"))
        return;

    if (std::ranges::any_of(decryptionKey, [] (auto const c) { return c != 0x00; })) [[likely]]
        decrypt({readBuffer.data() + sizeof(PacketHeader), packetBodySize});
    else
        decryptionKey = gEncryptionKey;

    if (packetHandler)
        packetHandler(readBuffer);
}

bool Connection::ConnectionImpl::handleReadError(boost::system::error_code const & ec, std::string_view source)
{
    switch (auto const code = ec.value(); code)
    {
        using enum boost::system::errc::errc_t;
        using namespace boost::asio::error;

        case success:
            return true;

        case boost::asio::error::connection_reset: // Client closed the connection
            SPDLOG_WARN("connection reset during packet {} reading: {} ({}:{})",
                        source, ec.message(), ec.category().name(), code);
            [[fallthrough]];

        case no_such_file_or_directory: // EOF
            SPDLOG_INFO("Client disconnected");
            return false;

        default:
        {
            SPDLOG_ERROR("Failed to read next packet {} (code {}): {}", source, code, ec.message());
            close();
            return false;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Connection::Connection(boost::asio::ip::tcp::socket && socket)
    : _impl(std::move(socket))
{}

Connection::~Connection()
{
    _impl->close();
}

void Connection::asyncReadNextPacket()
{
    if (!isAlive())
        return;

    boost::asio::async_read(_impl->socket, boost::asio::buffer(_impl->readBuffer.data(), sizeof(PacketHeader)),
                            [this] (auto const & ec, auto sz) { _impl->onSizeRead(ec, sz); });
}

void Connection::send(l2cpp::Network::Packet & p)
{
    if (!isAlive())
        return;

    L2CPP_B_ASSERT(p.opCode().has_value(), "Trying to send a packet without any opCode");
    auto const opCode = p.opCode().value();

    p.finalize();

#ifndef NDEBUG
    SPDLOG_INFO("sent: 0x{:0{}x} ({} bytes)", opCode, opCode > 0xff ? 4 : 2, p.size());
    std::cout << l2cpp::hexdump(p.body().data(), p.bodySize()) << std::endl;
#endif

    if (std::ranges::any_of(_impl->encryptionKey, [] (auto const c) { return c != 0x00; })) [[likely]]
        _impl->encrypt(p.body());
    else
        _impl->encryptionKey = gEncryptionKey;

    boost::system::error_code ec;
    boost::asio::write(_impl->socket, boost::asio::buffer(p.buffer()), ec);
    if (ec)
        SPDLOG_ERROR("send error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

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
    _impl->close();
}

void Connection::setOnPacketReceivedHandler(PacketReceivedHandler handler)
{
    _impl->packetHandler = std::move(handler);
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
