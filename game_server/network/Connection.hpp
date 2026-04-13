/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

// Third-party includes
#include <boost/asio/ip/tcp.hpp>

// C++ includes
#include <span>

namespace l2cpp::Network { class Packet; }
namespace Network { class Connection; }

class Network::Connection
{
public:
    using ConnectionClosedHandler = std::function<void()>;
    using PacketReceivedHandler   = std::function<void(std::span<byte const>)>;

public:
    explicit Connection(boost::asio::ip::tcp::socket && socket);
    ~Connection();

public:
    auto id()            const -> u64;
    bool isAlive()       const;
    auto readBuffer()    const -> std::span<byte const>;
    auto encryptionKey() const -> std::span<byte const>;

public:
    void asyncReadNextPacket();
    void send(l2cpp::Network::Packet & p, std::source_location const & src = std::source_location::current());
    void send(l2cpp::Network::Packet && p,
              std::source_location const & src = std::source_location::current()) { send(p, src); }
    void close();

    void setOnConnectionClosed(ConnectionClosedHandler);
    void setOnPacketReceivedHandler(PacketReceivedHandler);

private:
    struct ConnectionImpl;
    Pimpl<ConnectionImpl> _impl;
};
