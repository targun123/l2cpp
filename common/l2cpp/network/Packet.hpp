/// @author    Chnossos
/// @date      Created on 2026-02-17

#pragma once

// Project includes
#include "../utils/Traits.hpp"
#include "../Pimpl.hpp"
#include "../Typedefs.hpp"

// C++ includes
#include <optional>
#include <span>

namespace l2cpp::Network { class Packet; template<u16> struct HeaderOnlyPacket; }

class l2cpp::Network::Packet
{
public:
    explicit Packet(PacketOpCode opCode);

    /// Permits conversion from any Packet enumerations that has the correct underlying type size
    template<typename E>
    requires std::is_enum_v<E> && Utils::Traits::isAnyOf<std::underlying_type_t<E>, byte, PacketOpCode>
    explicit Packet(E e): Packet(std::to_underlying(e)) {}

    virtual ~Packet();

public:
    /// Appends a span of bytes to the packet.
    Packet & operator<<(std::span<byte const> span);

    template<typename T, size_t N>
    Packet & operator<<(std::span<T, N> span) { return append(span.data(), span.size() * sizeof(T)); }

    /// Allows to append any "basic" type as bytes to the packet.
    template<typename T> requires std::integral<T> || std::floating_point<T> || std::is_enum_v<T>
    Packet & operator<<(T t)
    {
        if constexpr (std::is_enum_v<T>)
            return append(&t, sizeof(std::underlying_type_t<T>));
        else
            return append(&t, sizeof(T));
    }

    /// Appends a contiguous array of integrals to the packet.
    template<typename T, size_t N> requires std::integral<T> || std::floating_point<T>
    Packet & operator<<(std::array<T, N> const & a)
    {
        return append(a.data(), a.size() * sizeof(T));
    }

    Packet & operator<<(std::string_view const str)
    {
        return append(str.data(), str.size() * sizeof(char)) << '\0';
    }

    Packet & operator<<(std::wstring_view const str)
    {
        return append(str.data(), str.size() * sizeof(wchar_t)) << L'\0';
    }

public:
    /// Writes the total size at the beginning.
    void writeSize();

public:
    /// @returns A read-only span of the whole buffer.
    auto buffer() const -> std::span<byte const>;

    /// @returns Size of the whole buffer.
    auto size() const -> size_t;

    /// @returns Opcode of the packet, if available.
    auto opCode() const -> std::optional<PacketOpCode>;

    /// @returns A span of the buffer minus the initial size (thus including the opCode).
    auto body()       -> std::span<byte>;
    auto body() const -> std::span<byte const>;

    /// @returns Size of the body (buffer size minus the initial size).
    auto bodySize() const -> size_t;

private:
    template<typename T>
    Packet & append(T const * ptr, size_t const sz)
    {
        return operator<<({reinterpret_cast<byte const *>(ptr), sz});
    }

private:
    struct PacketImpl;
    Pimpl<PacketImpl> _impl;
};

template<u16 OpCode>
struct l2cpp::Network::HeaderOnlyPacket : public Packet
{
    HeaderOnlyPacket()
        : Packet(OpCode)
    {}
};

template<typename T> requires std::is_class_v<T>
l2cpp::Network::Packet & operator<<(l2cpp::Network::Packet && p, T const & t)
{
    return p << t;
}
