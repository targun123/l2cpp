/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/BodyPart.hpp"
#include "../game/Item.hpp"

static u16 typeFromCategory(l2::ItemCategory cat)
{
    using enum l2::ItemCategory;
    switch (cat)
    {
        case Weapon:
        case Accessory:
            return 0;

        case Armor:
            return 1;

        case Quest:
        case Adena:
        case Misc:
            return 4;

        default:
            L2CPP_THROW("Unknown item category '{}' for type conversion", std::to_underlying(cat));
    }
};

Packet & operator<<(Packet & p, l2::Item const & item)
{
    return p
        << typeFromCategory(item.tmplate.category)
        << item.uid
        << item.tmplate.id
        << item.quantity
        << item.tmplate.category
        << 0_u16
        << 0_u16 // equipped?
        << item.tmplate.bodyPart
        << 0_u16 // enchant level (or pet level shown in control item)
        << 0_u16 // pet name exists or not shown in control item
    ;
}

DEFINE_PACKET_HANDLER(InventoryOpen)
{
    l2::Item item;
    item.uid              = 2;
    item.tmplate.id       = 6611; // Infinity Sword
    item.tmplate.bodyPart = l2::BodyPart::RightHand;
    item.tmplate.category = l2::ItemCategory::Weapon;

    Packet p(0x1b);
    p
        << 1_u16 // open
        << 1_u16 // count
        << item
    ;
    player.connection().send(p);
}
