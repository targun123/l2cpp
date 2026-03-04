/// @author    Chnossos
/// @date      Created on 2026-02-17

#include "Packet.hpp"

// Project includes
#include "../Exception.hpp"
#include "../details/Pimpl.hpp"

// C++ includes
#include <mutex>
#include <vector>

using l2cpp::Network::Packet;

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

Packet::Packet(PacketOpCode const opCode)
{
    constexpr PacketHeader sizePlaceholder = 0;

    impl->buffer.reserve(256);
    *this << sizePlaceholder; // slot to write final size before sending

    if (opCode > 0xff)
        *this << opCode;
    else
        *this << static_cast<byte>(opCode);
        // impl->buffer.emplace_back(static_cast<byte>(opCode));
}

Packet::~Packet() = default;

Packet & Packet::operator<<(std::span<byte const> span)
{
    impl->buffer.append_range(span);
    return *this;
}

void Packet::writeChecksum()
{
    std::call_once(impl->checksumOnceFlag, [&, this]
    {
        // Align to 8 bytes then append checksum
        while (bodySize() % 8 != 0)
            impl->buffer.emplace_back(0);

        *this << calculateChecksum({body().data(), bodySize()});
        *this << 0u; // Align to 8 bytes with zeroes, required by Blowfish
    });
}

void Packet::writeSize()
{
    // Write total size on the first two bytes of the buffer
    *reinterpret_cast<PacketHeader *>(impl->buffer.data()) = static_cast<PacketHeader>(impl->buffer.size());
}

auto Packet::buffer() const -> std::span<byte const>
{
    return impl->buffer;
}

auto Packet::size() const -> size_t
{
    return impl->buffer.size();
}

auto Packet::opCode() const -> std::optional<PacketOpCode>
{
    std::optional<PacketOpCode> type;

    if (impl->buffer.size() > sizeof(PacketHeader))
    {
        type = impl->buffer[sizeof(PacketHeader)];
        if (type == 0xfe)
            type = *reinterpret_cast<PacketOpCode const *>(impl->buffer.data() + sizeof(PacketHeader));
    }

    return type;
}

auto Packet::body() -> std::span<byte>
{
    return {impl->buffer.data() + sizeof(PacketHeader), bodySize()};
}

auto Packet::body() const -> std::span<byte const>
{
    return {impl->buffer.data() + sizeof(PacketHeader), bodySize()};
}

auto Packet::bodySize() const -> size_t
{
    return impl->buffer.size() - sizeof(PacketHeader);
}
