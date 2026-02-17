/// @author    Chnossos
/// @date      Created on 2026-02-17

#pragma once

#include "Packets.hpp"
#include <memory>
#include <optional>
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
    /// Appends a span of bytes to the packet.
    Packet & append(std::span<byte const> span);

    /// Appends a single byte to the packet.
    Packet & append(byte b) { return append({&b, 1}); }

    /// Shortened way to append to the packet.
    Packet & operator<<(auto && t) { return append(t); }

    /// Allows to append any integral type as bytes to the packet.
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Packet & append(T const & t) { return append({reinterpret_cast<byte const *>(&t), sizeof(T)}); }

    /// Appends a contiguous array of integrals as bytes to the packet.
    template<typename T, size_t N, typename = std::enable_if_t<std::is_integral_v<T>>>
    Packet & append(T const (&a)[N]) { return append({reinterpret_cast<byte const *>(a), N}); }

public:
    /// Appends the checksum of the packet body and writes the total size at the beginning.
    /// @note Does nothing if called more than once.
    void writeChecksumAndSize();

public:
    /// @returns A read-only span of the whole buffer.
    auto buffer() const -> std::span<byte const>;

    /// @returns Size of the whole buffer.
    auto size() const -> size_t;

    /// @returns Type of the packet if available, else @c 0xFF.
    auto type() const -> std::optional<u8>;

    /// @returns A span of the buffer minus the initial size (thus including the type).
    auto body() const -> std::span<byte>;

    /// @returns Size of the body (buffer size minus the initial size).
    auto bodySize() const -> size_t;

private:
    struct PacketImpl;
    std::unique_ptr<PacketImpl> impl;
};
