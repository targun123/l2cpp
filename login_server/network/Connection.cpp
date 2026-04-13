/// @author    Chnossos
/// @date      Created on 2026-04-11

#include "Connection.hpp"

// Project includes
#include "../crypto/Blowfish.hpp"
#include "../crypto/Checksum.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/network/Packet.hpp>

// Third-party includes
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

using boost::asio::ip::tcp;
using l2cpp::Network::Packet;

Connection::Connection(tcp::socket socket)
    : socket(std::move(socket))
{
    static decltype(sessionId) nextSessionId = 0;
    sessionId                                = nextSessionId++;

    readBuffer.resize(sizeof(u16));
}

bool Connection::operator==(Connection const & other) const { return sessionId == other.sessionId; }

void Connection::close()
{
    if (socket.is_open())
    {
        boost::system::error_code ec;
        if (socket.shutdown(socket.shutdown_both, ec))
            SPDLOG_ERROR("shutdown error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

        if (socket.close(ec))
            SPDLOG_ERROR("close error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

        if (onSocketClosed)
            onSocketClosed();
    }
}

void Connection::asyncReadNextPacket()
{
    L2CPP_B_ASSERT(socket.is_open(), "Cannot read another packet: socket not open");
    boost::asio::async_read(socket, boost::asio::buffer(readBuffer, sizeof(PacketHeader)),
                            [this] (auto const & ec, auto) { onSizeRead(ec); });
}

void Connection::send(Packet & p, bool const encryptPacket)
{
    static std::array<byte, 7> pad{};

    // Align to 8 bytes then append checksum
    if (auto const overflow = p.bodySize() % 8)
        p << std::span(pad.data(), 8 - overflow);

    p << Checksum::calculate({p.body().data(), p.bodySize()});

    if (encryptPacket)
        Blowfish::encrypt(p.body());

    boost::asio::write(socket, boost::asio::buffer(p.buffer()));
    SPDLOG_INFO("'{}' ← 0x{:02x} ({:>3} bytes)", sessionId, p.opCode(), p.size());
}

void Connection::onSizeRead(boost::system::error_code const & ec)
{
    if (!handleError(ec))
        return close();

    readBuffer.resize(*reinterpret_cast<PacketHeader const *>(readBuffer.data()));

    auto const payload = std::span(readBuffer).subspan(sizeof(PacketHeader));
    boost::asio::async_read(socket, boost::asio::buffer(payload), [this] (auto const & e, auto) { onBodyRead(e); });
}

void Connection::onBodyRead(boost::system::error_code const & ec)
{
    if (!handleError(ec))
        return close();

    if (onPacketRead)
        onPacketRead(readBuffer);
}

bool Connection::handleError(boost::system::error_code const & ec) const
{
    switch (auto const code = ec.value())
    {
        case boost::system::errc::success:
            return true;

        case boost::asio::error::eof:
            SPDLOG_INFO("Client {} disconnected by themselves", sessionId);
            break;

        case boost::asio::error::operation_aborted:
            SPDLOG_INFO("Client {} disconnected by server", sessionId);
            break;

        default:
            SPDLOG_ERROR("Something went wrong during packet body reading (code {}): {}", code, ec.message());
            break;
    }
    return false;
}
