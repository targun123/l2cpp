/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(Protocol)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    s32 protocol;
    reader >> protocol;

    /**/ if (protocol == -2)
        SPDLOG_INFO("Client ping");
    else if (protocol == 656)
        return player.connection().send(Packet(0x00) << true << player.connection().encryptionKey());
    else
    {
        SPDLOG_ERROR("Unsupported protocol {}, closing connection", protocol);
        player.connection().send(Packet(0x00) << false);
    }

    player.connection().close();
}
