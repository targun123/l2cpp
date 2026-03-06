/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"

DEFINE_PACKET_HANDLER(InventoryOpen)
{
    player.connection().send(InventoryListPacket(true, player.currentCharacter()->get().inventory()));
}
