/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../network/packets/server/client/ClientExitGamePacket.hpp"

DEFINE_PACKET_HANDLER(ConnectionClosing)
{
    if (player.currentCharacter()) // Client doesn't wait for an answer during character selection
        player.connection().send(ClientExitGamePacket());

    player.connection().close();
}
