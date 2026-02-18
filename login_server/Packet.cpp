/// @author    Chnossos
/// @date      Created on 2026-02-17

#include "Packet.hpp"

#include <mutex>
#include <vector>

struct Packet::PacketImpl
{
    std::vector<byte> buffer;
    std::once_flag    checksumOnceFlag;
};

Packet::Packet()
    : impl(std::make_unique<PacketImpl>())
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
    std::call_once(impl->checksumOnceFlag, [&, this]
    {
        // Pad to 4 bytes to perform checksum correctly
        while (bodySize() % sizeof(u32) != 0)
            impl->buffer.emplace_back(0);

        // Calculate checksum
        u32 checksum = 0;

        for (u32 i = 0; i < bodySize(); i += sizeof(u32))
            checksum ^= *reinterpret_cast<u32 const *>(body().data() + i);

        append(checksum);

        // Pad to 8 bytes with zeroes, required by Blowfish
        while (bodySize() % sizeof(u64) != 0)
            impl->buffer.emplace_back(0);

        // Write total size on the first two bytes of the buffer
        *reinterpret_cast<u16 *>(impl->buffer.data()) = static_cast<u16>(impl->buffer.size());
    });
}

auto Packet::buffer() const -> std::span<byte const>
{
    return impl->buffer;
}

auto Packet::size() const -> size_t
{
    return impl->buffer.size();
}

auto Packet::type() const -> std::optional<u8>
{
    std::optional<u8> type;

    if (impl->buffer.size() > sizeof(u16))
        type.emplace(impl->buffer[sizeof(u16)]);

    return type;
}

auto Packet::body() const -> std::span<byte>
{
    return {impl->buffer.data() + sizeof(u16), bodySize()};
}

auto Packet::bodySize() const -> size_t
{
    return impl->buffer.size() - sizeof(u16);
}
