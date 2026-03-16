/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"


DEFINE_PACKET_HANDLER(EnterWorld)
{
    auto & c = player.currentCharacter()->get();
    player.connection().send(InventoryListPacket(false, c.inventory()));

    c.computeStats();
    player.connection().send(CharacterStatusUpdatePacket(c));
}
