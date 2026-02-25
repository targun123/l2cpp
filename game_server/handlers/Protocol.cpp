/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(Protocol)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 protocol;
    reader >> protocol;

    SPDLOG_INFO("Client protocol: {}", protocol);
    player.connection().send(Packet(0x00).append<u8>(1) << player.connection().encryptionKey());
}
