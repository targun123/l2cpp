/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/SkillDirectory.hpp"
#include "../network/packets/server/client/ClientForceDisconnectPacket.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdateBroadcastPacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(EnterWorld) try
{
    auto & c = *player.currentCharacter();
    player.connection().send(InventoryListPacket(false, c.inventory()));

    if (c.accessLevel > 0)
        c.skills().learn(7029, 4); // Super Haste

    c.computeStats();
    player.connection().send(CharacterStatusUpdatePacket(c));
    World::broadcastAround(c, CharacterStatusUpdateBroadcastPacket(c));
}
catch (...)
{
    // Any exception would get the client stuck during the login screen; force disconnect him instead.
    player.connection().send(ClientForceDisconnectPacket());
    player.connection().close();
    throw;
}
