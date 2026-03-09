/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(ItemUnequip)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "No current player, can't unequip item");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    l2::GearSlot slot;
    reader >> slot;

    auto & c = player.currentCharacter()->get();
    if (auto const item = c.gearItem(slot); item)
    {
        if (auto const transaction = c.unequipItem(item->get()); transaction.succeeded)
        {
            InventoryUpdatePacket p;
            p.appendModifiedItem(item->get());

            player.connection().send(p);
            player.connection().send(CharacterStatusUpdatePacket(c));
        }
    }
}
