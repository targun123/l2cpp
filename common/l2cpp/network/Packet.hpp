/// @author    Chnossos
/// @date      Created on 2026-02-17

#pragma once

// Project includes
#include "../Pimpl.hpp"
#include "../Typedefs.hpp"

// C++ includes
#include <optional>
#include <span>
#include <string>

namespace l2cpp::Network { class Packet; }

class l2cpp::Network::Packet
{
public:
    Packet();
    explicit Packet(byte type);
    /// Permits conversion from any Packet enumerations
    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    explicit Packet(E e): Packet(std::to_underlying(e)) {}
    virtual ~Packet();

public:
    /// Appends a span of bytes to the packet.
    Packet & append(std::span<byte const> span);

    /// Allows to append any integral type as bytes to the packet.
    template<typename T,
             typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>>>
    Packet & append(T t)
    {
        if constexpr (std::is_enum_v<T>)
            return append(std::to_underlying(t));
        else
            return append({reinterpret_cast<byte const *>(&t), sizeof(T)});
    }

    /// Appends a contiguous array of integrals as bytes to the packet.
    template<typename T, size_t N, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
    Packet & append(T const (&a)[N])
    {
        return append({reinterpret_cast<byte const *>(a), N * sizeof(T)});
    }

    /// Appends a contiguous array of integrals to the packet
    template<typename T, size_t N, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
    Packet & append(std::array<T, N> const & a)
    {
        return append({reinterpret_cast<byte const *>(a.data()), a.size() * sizeof(T)});
    }

    Packet & append(std::string_view const str)
    {
        return append({reinterpret_cast<byte const *>(str.data()), str.size() * sizeof(char)}).append<char>(0);
    }

    Packet & append(std::wstring_view const str)
    {
        return append({reinterpret_cast<byte const *>(str.data()), str.size() * sizeof(wchar_t)}).append<wchar_t>(0);
    }

    /// Shortened way to append to the packet.
    Packet & operator<<(auto && t) { return append(t); }

public:
    /// Appends the checksum of the packet body.
    /// @note Does nothing if called more than once.
    void writeChecksum();

    /// Writes the total size at the beginning.
    void writeSize();

public:
    /// @returns A read-only span of the whole buffer.
    auto buffer() const -> std::span<byte const>;

    /// @returns Size of the whole buffer.
    auto size() const -> size_t;

    /// @returns Opcode of the packet, if available.
    auto opCode() const -> std::optional<u8>;

    /// @returns A span of the buffer minus the initial size (thus including the opCode).
    auto body()       -> std::span<byte>;
    auto body() const -> std::span<byte const>;

    /// @returns Size of the body (buffer size minus the initial size).
    auto bodySize() const -> size_t;

private:
    struct PacketImpl;
    Pimpl<PacketImpl> impl;
};
