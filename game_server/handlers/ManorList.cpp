/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(ManorList)
{
    player.connection().send(Packet(0xfe1b) << 0);
}
