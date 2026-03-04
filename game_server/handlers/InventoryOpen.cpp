/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(InventoryOpen)
{
    Packet p(0x1b);
    p
        << u16(1) // open
        << u16(0) // count
    ;
    player.connection().send(p);
}
