/// @author    Chnossos
/// @date      Created on 2026-02-17

#include "Packet.hpp"

// Project includes
#include "../details/Pimpl.hpp"

// C++ includes
#include <vector>

using l2cpp::Network::Packet;

struct Packet::PacketImpl
{
    std::vector<byte> buffer;
    bool              isFinal = false;

    PacketImpl(PacketOpCode const opCode);

    template<typename T> requires(std::integral<T> || std::floating_point<T>)
    PacketImpl & append(T t)
    {
        buffer.append_range(std::span{reinterpret_cast<byte const *>(&t), sizeof(T)});
        return *this;
    }

    template<typename T, typename U = T> requires(std::integral<U> || std::floating_point<U>)
    PacketImpl & append(U u)
    {
        return append(static_cast<T>(u));
    }
};

template class Pimpl<Packet::PacketImpl>;

Packet::PacketImpl::PacketImpl(PacketOpCode const opCode)
{
    buffer.reserve(256);
    append<u16>(0); // placeholder for final packet size

    if (opCode > 0xff)
        append(opCode);
    else
        append<u8>(opCode);
}

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(PacketOpCode const opCode)
    : _impl(opCode)
{}

Packet::~Packet() = default;

Packet & Packet::operator<<(std::span<byte const> span)
{
    if (!_impl->isFinal)
        _impl->buffer.append_range(span);

    return *this;
}

void Packet::writeSize()
{
    // Write total size on the first two bytes of the buffer
    *reinterpret_cast<PacketHeader *>(_impl->buffer.data()) = static_cast<PacketHeader>(_impl->buffer.size());
}

auto Packet::buffer() const -> std::span<byte const>
{
    return _impl->buffer;
}

auto Packet::size() const -> size_t
{
    return _impl->buffer.size();
}

auto Packet::opCode() const -> std::optional<PacketOpCode>
{
    std::optional<PacketOpCode> type;

    if (_impl->buffer.size() > sizeof(PacketHeader))
    {
        type = _impl->buffer[sizeof(PacketHeader)];
        if (type == 0xfe)
            type = *reinterpret_cast<PacketOpCode const *>(_impl->buffer.data() + sizeof(PacketHeader));
    }

    return type;
}

auto Packet::body() -> std::span<byte>
{
    return {_impl->buffer.data() + sizeof(PacketHeader), bodySize()};
}

auto Packet::body() const -> std::span<byte const>
{
    return {_impl->buffer.data() + sizeof(PacketHeader), bodySize()};
}

auto Packet::bodySize() const -> size_t
{
    return _impl->buffer.size() - sizeof(PacketHeader);
}

void Packet::finalize()
{
    if (!_impl->isFinal)
    {
        finalizeImpl();
        writeSize();
        _impl->isFinal = true;
    }
}
