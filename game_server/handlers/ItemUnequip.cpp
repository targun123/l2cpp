/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/Gear.hpp"
#include "../network/packets/server/status/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryUpdatePacket.hpp"

// C++ includes
#include <ranges>

DEFINE_PACKET_HANDLER(ItemUnequip)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "No current player, can't unequip item");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    GearSlot slot;
    reader >> slot;

    auto & c = *player.currentCharacter();
    if (auto const item = c.gear().item(slot); item)
    {
        // FIXME: if any kind of arrow
        Ref const finalitem = item->tmplate.id == 1345 ? *c.gear().item(GearSlot::RightHand) : *item;

        if (auto const transaction = c.gear().unequipItem(finalitem); transaction.succeeded)
        {
            InventoryUpdatePacket p;

            for (auto const & i : transaction.oldItems | std::views::values)
                p.appendModifiedItem(i);

            player.connection().send(p);
            World::broadcastAround(c, CharacterStatusUpdatePacket(c), true);
        }
    }
}
