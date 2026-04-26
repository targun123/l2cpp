/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/actor/Npc.hpp"
#include "../game/components/ActorAutoRegen.hpp"
#include "../game/components/Stats.hpp"
#include "../network/packets/server/chat/ChatSystemSayPacket.hpp"
#include "../network/packets/server/client/ClientForceDisconnectPacket.hpp"
#include "../network/packets/server/inventory/InventoryListPacket.hpp"
#include "../network/packets/server/status/ActorDiePacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdateBroadcastPacket.hpp"
#include "../network/packets/server/status/CharacterStatusUpdatePacket.hpp"
#include "../network/packets/server/status/NpcStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(EnterWorld) try
{
    auto & c    = *player.currentCharacter();
    auto & conn = player.connection();

    conn.send(InventoryListPacket(false, c.inventory()));
    conn.send(ChatSystemSayPacket(34)); // Welcome to the world of Lineage II

    c.stats().compute(c);
    if (c.isAlive())
        c.addComponent<ActorAutoRegen>();

    World::forEachActorAround(c, [&] (Actor & a)
    {
        if (a.type() == ActorType::Character)
            conn.send(CharacterStatusUpdateBroadcastPacket{static_cast<Character &>(a)});
        else
            conn.send(NpcStatusUpdatePacket{static_cast<Npc &>(a)});
    });
    conn.send(CharacterStatusUpdatePacket(c));
    if (!c.isAlive())
        conn.send(ActorDiePacket{c});
    World::broadcastAround(c, CharacterStatusUpdateBroadcastPacket(c));
}
catch (...)
{
    // Any exception would get the client stuck during the login screen; force disconnect him instead.
    player.connection().send(ClientForceDisconnectPacket());
    player.connection().close();
    throw;
}
