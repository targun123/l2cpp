/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(ConnectionClosing)
{
    if (!player.characters().empty())
        player.connection().send(Packet() << 0x7e);

    player.connection().close();
}
