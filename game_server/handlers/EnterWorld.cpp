/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/SkillDirectory.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(EnterWorld)
{
    auto & c = player.currentCharacter()->get();
    player.connection().send(InventoryListPacket(false, c.inventory()));

    if (c.accessLevel > 0)
        c.skills().learn(7029, 4); // Super Haste

    c.computeStats();
    player.connection().send(CharacterStatusUpdatePacket(c));
}
