/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/Gear.hpp"
#include "../game/inventory/ItemStorage.hpp"
#include "../network/packets/server/status/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryUpdatePacket.hpp"

// C++ includes
#include <ranges>

DEFINE_PACKET_HANDLER(ItemUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    GameObjectId uid;
    bool ctrlPressed;
    reader >> uid >> ctrlPressed;

    auto & character = *player.currentCharacter();
    auto const item  = character.inventory().item(uid);
    if (!item)
        return;

    switch (item->tmplate.category)
    {
        case ItemCategory::Weapon:
        case ItemCategory::Armor:
        case ItemCategory::Accessory:
        {
            if (auto const transaction = character.gear().equipItem(*item); transaction.succeeded)
            {
                InventoryUpdatePacket p;
                if (!transaction.oldItems.empty())
                {
                    for (auto const & oldItem : transaction.oldItems | std::views::values)
                        p.appendModifiedItem(oldItem);
                }

                if (transaction.curItem)
                    p.appendModifiedItem(*transaction.curItem);

                if (item->tmplate.id == 6619) // FIXME: if any bow, find matching arrows to equip in left hand
                {
                    if (auto const matchingArrows = character.inventory().item({.id = 1345}); !matchingArrows.empty())
                    {
                        auto const subTr = character.gear().equipItem(matchingArrows.front());
                        p.appendModifiedItem(*subTr.curItem);
                    }
                }

                player.connection().send(p);
                World::broadcastAround(character, CharacterStatusUpdatePacket(character), true);
            }
            break;
        }
        case ItemCategory::Misc:
            // Do something if item is usable (e.g. potions)
            break;

        default:
            break;
    }
}
