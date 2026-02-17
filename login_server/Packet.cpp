/// @author    Chnossos
/// @date      Created on 2026-02-17

#include "Packet.hpp"

#include <vector>

struct Packet::PacketImpl
{
    std::vector<byte> buffer;
};

Packet::Packet()
    : impl(new PacketImpl())
{
    impl->buffer.reserve(256);
    append<u16>(0); // slot to write final size before sending
}

Packet::Packet(byte const type)
    : Packet()
{
    impl->buffer.emplace_back(type);
}

Packet::Packet(RecvPacket const type)
    : Packet(static_cast<byte>(type))
{}

Packet::Packet(SentPacket const type)
    : Packet(static_cast<byte>(type))
{}

Packet::~Packet() = default;

Packet & Packet::append(std::span<byte const> span)
{
    impl->buffer.append_range(span);
    return *this;
}

void Packet::writeChecksumAndSize()
{
    u32 checksum = 0;
    for (auto start = body().data(), end = start + size(); start + sizeof(u32) < end; )
    {
        u32 ecx  = (*start++ << 0x00) & 0xFF;
            ecx |= (*start++ << 0x08) & 0xFF00;
            ecx |= (*start++ << 0x10) & 0xFF0000;
            ecx |= (*start++ << 0x18) & 0xFF000000;

        checksum ^= ecx;
    }
    append(checksum);

    while (bodySize() % 8 != 0) // Pad to 8 bytes with zeroes
        impl->buffer.emplace_back(0);

    // Write total size on the first two bytes of the buffer
    auto const finalSize = static_cast<u16>(size());
    std::memcpy(impl->buffer.data(), &finalSize, sizeof(finalSize));
}

auto Packet::buffer() const -> std::span<byte const>
{
    return impl->buffer;
}

auto Packet::size() const -> size_t
{
    return impl->buffer.size();
}

auto Packet::type() const -> u8
{
    return impl->buffer.size() > sizeof(u16) ? impl->buffer[sizeof(u16)] : 0xFF;
}

auto Packet::body() const -> std::span<byte const>
{
    return {impl->buffer.data() + sizeof(u16), bodySize()};
}

auto Packet::bodySize() const -> size_t
{
    return impl->buffer.size() - sizeof(u16);
}
