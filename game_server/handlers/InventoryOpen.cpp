/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/BodyPart.hpp"
#include "../game/Character.hpp"
#include "../game/Item.hpp"

DEFINE_PACKET_HANDLER(InventoryOpen)
{
    auto const inventory = player.currentCharacter()->get().inventory();

    Packet p(0x1b);
    p
        << 1_u16 // open
        << static_cast<u16>(inventory.size())
    ;

    for (auto const & item : inventory)
        p << item.get();

    player.connection().send(p);
}
