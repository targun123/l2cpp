/// @author    Chnossos
/// @date      Created on 2026-02-17

#pragma once

#include "Packets.hpp"
#include <memory>
#include <span>

class Packet
{
public:
    Packet();
    explicit Packet(byte       type);
    explicit Packet(RecvPacket type);
    explicit Packet(SentPacket type);
    ~Packet();

public:
    Packet & append(std::span<byte const> span);
    Packet & append(byte b)        { return append({&b, 1}); }
    Packet & operator<<(auto && t) { return append(t);       }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Packet & append(T const & t) { return append({reinterpret_cast<byte const *>(&t), sizeof(T)}); }

    template<typename T, std::size_t N, typename = std::enable_if_t<std::is_integral_v<T>>>
    Packet & append(T const (&a)[N]) { return append({reinterpret_cast<byte const *>(a), N}); }

public:
    void writeChecksumAndSize();

public:
    auto buffer()   const -> std::span<byte const>;
    auto size()     const -> size_t;
    auto type()     const -> u8;

    auto body()     const -> std::span<byte const>;
    auto bodySize() const -> size_t;

private:
    struct PacketImpl;
    std::unique_ptr<PacketImpl> impl;
};
