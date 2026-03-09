/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../game/inventory/Item.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryUpdatePacket.hpp"

// C++ includes
#include <ranges>

DEFINE_PACKET_HANDLER(ItemUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    GameObjectId uid;
    bool ctrlPressed;
    reader >> uid >> ctrlPressed;

    auto & character = player.currentCharacter()->get();
    auto const item  = character.inventory().item(uid);
    if (!item)
        return;

    switch (item->get().tmplate.category)
    {
        case l2::ItemCategory::Weapon:
        case l2::ItemCategory::Armor:
        case l2::ItemCategory::Accessory:
        {
            if (auto const transaction = character.gear().equipItem(item->get()); transaction.succeeded)
            {
                InventoryUpdatePacket p;
                if (!transaction.oldItems.empty())
                {
                    for (auto const & oldItem : transaction.oldItems | std::views::values)
                        p.appendModifiedItem(oldItem.get());
                }

                if (transaction.curItem)
                    p.appendModifiedItem(*transaction.curItem);

                player.connection().send(p);
                player.connection().send(CharacterStatusUpdatePacket(character));
            }
            break;
        }
        case l2::ItemCategory::Quest:
            break;
        case l2::ItemCategory::Adena:
            break;
        case l2::ItemCategory::Misc:
            break;

        default:
            break;
    }
}
