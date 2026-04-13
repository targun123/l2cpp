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
    bool              isFinalized = false;
    PacketOpCode      opCode;

    explicit PacketImpl(PacketOpCode opCode_);
    PacketImpl(PacketImpl const & other);

    template<typename T> requires std::integral<T> || std::floating_point<T>
    PacketImpl & append(T t)
    {
        buffer.append_range(std::span{reinterpret_cast<byte const *>(&t), sizeof(T)});
        return *this;
    }

    template<typename T, typename U = T> requires std::integral<U> || std::floating_point<U>
    PacketImpl & append(U u)
    {
        return append(static_cast<T>(u));
    }
};

template class Pimpl<Packet::PacketImpl>;

Packet::PacketImpl::PacketImpl(PacketOpCode const opCode_)
    : opCode(opCode_)
{
    buffer.reserve(256);
    append<u16>(0); // placeholder for final packet size

    if (opCode > 0xff)
        append(opCode);
    else
        append<u8>(opCode);
}

Packet::PacketImpl::PacketImpl(PacketImpl const & other)
    : buffer(other.buffer)
    , isFinalized(other.isFinalized)
    , opCode(other.opCode)
{}

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(PacketOpCode const opCode)
    : _impl(opCode)
{}

Packet::Packet(Packet const & other)
    : _impl(*other._impl)
{}

Packet::~Packet() = default;

bool Packet::isFinalized() const                          { return _impl->isFinalized;   }
auto Packet::buffer()      const -> std::span<byte const> { return _impl->buffer;        }
auto Packet::size()        const -> size_t                { return _impl->buffer.size(); }
auto Packet::opCode()      const -> PacketOpCode          { return _impl->opCode;        }

auto Packet::body()       -> std::span<byte>       { return {_impl->buffer.data() + sizeof(PacketHeader), bodySize()}; }
auto Packet::body() const -> std::span<byte const> { return {_impl->buffer.data() + sizeof(PacketHeader), bodySize()}; }
auto Packet::bodySize() const -> size_t            { return  _impl->buffer.size() - sizeof(PacketHeader);              }

void Packet::finalize()
{
    if (!_impl->isFinalized)
    {
        finalizeImpl();
        writeSize();
        _impl->isFinalized = true;
    }
}

Packet & Packet::operator<<(std::span<byte const> span)
{
    if (!_impl->isFinalized)
        _impl->buffer.append_range(span);

    return *this;
}

void Packet::writeSize()
{
    // Write total size on the first two bytes of the buffer
    *reinterpret_cast<PacketHeader *>(_impl->buffer.data()) = static_cast<PacketHeader>(_impl->buffer.size());
}
