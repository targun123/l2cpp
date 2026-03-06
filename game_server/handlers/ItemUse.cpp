/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../game/Item.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(ItemUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    GameObjectId uid;
    bool ctrlPressed;
    reader >> uid >> ctrlPressed;

    auto & c = player.currentCharacter()->get();
    auto const inventory = c.inventory();
    auto const it = std::ranges::find_if(inventory, [uid] (auto const & item) { return item.get().id() == uid; });
    if (it != inventory.cend())
    {
        switch (it->get().tmplate.category)
        {
            case l2::ItemCategory::Unknown: return;
            case l2::ItemCategory::Weapon:
            case l2::ItemCategory::Armor:
            case l2::ItemCategory::Accessory:
            {
                auto const [oldItem, newItem] = c.equipItem(uid);
                if (!oldItem && !newItem) // nothing changed
                    return;

                Packet p(0x27);
                p << static_cast<u16>(!!oldItem + !!newItem);
                if (oldItem) p << 2_u16 << oldItem->get();
                if (newItem) p << 2_u16 << newItem->get();
                player.connection().send(p);
                player.connection().send(Network::Packet::Server::CharacterStatusUpdatePacket(c));
                break;
            }
            case l2::ItemCategory::Quest:
                break;
            case l2::ItemCategory::Adena:
                break;
            case l2::ItemCategory::Misc:
                break;
            case l2::ItemCategory::Count:
                break;
        }
    }
}
