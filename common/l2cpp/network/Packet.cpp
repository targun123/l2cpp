/// @author    Chnossos
/// @date      Created on 2026-02-17

#include "Packet.hpp"

// Project includes
#include "../Exception.hpp"

// C++ includes
#include <mutex>
#include <vector>

namespace
{
    u32 calculateChecksum(std::span<byte const> data)
    {
        L2CPP_B_ASSERT(data.size() >= sizeof(u32), "Cannot calculate checksum: size < sizeof(u32)");

        u32 checksum = 0;

        for (size_t i = 0; i < data.size(); i += sizeof(u32))
            checksum ^= *reinterpret_cast<u32 const *>(data.data() + i);

        return checksum;
    }

    // TODO: verifyChecksum
}

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

Packet::~Packet() = default;

Packet & Packet::append(std::span<byte const> span)
{
    impl->buffer.append_range(span);
    return *this;
}

void Packet::writeChecksum()
{
    std::call_once(impl->checksumOnceFlag, [&, this]
    {
        // Align to 4 bytes to perform checksum correctly
        while (bodySize() % sizeof(u32) != 0)
            impl->buffer.emplace_back(0);

        append(calculateChecksum({body().data(), bodySize()}));

        if (bodySize() % sizeof(u64))
            append<u32>(0); // Align to 8 bytes with zeroes, required by Blowfish
    });
}

void Packet::writeSize()
{
    // Write total size on the first two bytes of the buffer
    *reinterpret_cast<u16 *>(impl->buffer.data()) = static_cast<u16>(impl->buffer.size());
}

auto Packet::buffer() const -> std::span<byte const>
{
    return impl->buffer;
}

auto Packet::size() const -> size_t
{
    return impl->buffer.size();
}

auto Packet::opCode() const -> std::optional<u8>
{
    std::optional<u8> type;

    if (impl->buffer.size() > sizeof(u16))
        type.emplace(impl->buffer[sizeof(u16)]);

    return type;
}

auto Packet::body() -> std::span<byte>
{
    return {impl->buffer.data() + sizeof(u16), bodySize()};
}

auto Packet::body() const -> std::span<byte const>
{
    return {impl->buffer.data() + sizeof(u16), bodySize()};
}

auto Packet::bodySize() const -> size_t
{
    return impl->buffer.size() - sizeof(u16);
}
