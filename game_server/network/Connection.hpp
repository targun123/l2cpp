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
    explicit Connection(boost::asio::ip::tcp::socket && socket);
    ~Connection();

public:
    void read(bool decrypt = true);
    void send(l2cpp::Network::Packet & p, bool encrypt = true);
    void close();

public:
    bool isAlive()       const;
    auto readBuffer()    const -> std::span<byte const>;
    auto encryptionKey() const -> std::span<byte const>;

private:
    struct ConnectionImpl;
    Pimpl<ConnectionImpl> _impl;
};
