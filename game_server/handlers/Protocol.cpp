/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(Protocol)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    i32 protocol;
    reader >> protocol;

    /**/ if (protocol == -2)
        SPDLOG_INFO("Client ping");
    else if (protocol == 656)
    {
        SPDLOG_INFO("Client protocol: {}", protocol);
        player.connection().send(Packet(0x00).append<u8>(1) << player.connection().encryptionKey());
        return;
    }
    else
        SPDLOG_ERROR("Unsupported protocol {}, closing connection", protocol);

    player.connection().close();
}
