/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../game/Item.hpp"

DEFINE_PACKET_HANDLER(ItemUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 itemUid;
    bool ctrlPressed;
    reader >> itemUid >> ctrlPressed;

    auto const inventory = player.currentCharacter()->get().inventory();
    auto const it = std::ranges::find_if(inventory, [itemUid](auto const & item)
    {
        return item.get().id() == itemUid;
    });
    if (it != inventory.cend())
    {
        Packet p(0x27); // Inventory update
        p
            << 1_u16 // count?
            << 2_u16 // mode: 2=update
            << *it
        ;
        player.connection().send(p);
    }
}
