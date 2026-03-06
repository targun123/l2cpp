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

    l2::BodyPart bodyPart;
    reader >> bodyPart;

    auto & c = player.currentCharacter()->get();
    if (auto const item = c.unequipItem(bodyPart); item)
    {
        InventoryUpdatePacket p;
        p.appendModifiedItem(item->get());

        player.connection().send(p);
        player.connection().send(CharacterStatusUpdatePacket(c));
    }
}
