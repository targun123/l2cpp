/// @author    Chnossos
/// @date      Created on 2026-03-06

#pragma once

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(ItemUnequip)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "No current player, can't unequip item");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    l2::BodyPart bodyPart;
    reader >> bodyPart;

    auto & c = player.currentCharacter()->get();
    if (auto const item = c.unequipItem(bodyPart); item)
    {
        Packet p(0x27); // Inventory update
        p
            << 1_u16 // count?
            << 2_u16 // mode: 2=update
            << item->get()
        ;
        player.connection().send(p);
        player.connection().send(Network::Packet::Server::CharacterStatusUpdatePacket(c));
    }
}
