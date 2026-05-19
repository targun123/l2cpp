/// @author    Chnossos
/// @date      Created on 2026-02-28

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(MiniMapShow)
{
    player.connection().send(Packet(0x9d, "MiniMapShow") << 1665);
}
