/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>

// Third-party includes
#include <boost/asio/ip/tcp.hpp>

class Connection
{
public:
    explicit Connection(boost::asio::ip::tcp::socket && socket);
    ~Connection();

public:
    void read(bool needDecryption = true);
    void send(Packet & p, bool needEncryption = true);
    void close();

public:
    bool isAlive() const;
    auto readBuffer()    const -> std::span<byte const>;
    auto encryptionKey() const -> std::span<byte const>;

private:
    struct ConnectionImpl;
    Pimpl<ConnectionImpl> _impl;
};
