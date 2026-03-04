/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Item.hpp"

DEFINE_PACKET_HANDLER(ItemUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 itemUid;
    bool ctrlPressed;
    reader >> itemUid >> ctrlPressed;

    l2::Item item;
    item.uid              = 2;
    item.equipped         = true;
    item.tmplate.id       = 6611; // Infinity Sword
    item.tmplate.bodyPart = l2::BodyPart::RightHand;
    item.tmplate.category = l2::ItemCategory::Weapon;

    Packet p(0x27); // Inventory update
    p
        << 1_u16
        << 2_u16
        << item
    ;
    player.connection().send(p);
}
