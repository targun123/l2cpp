#include "ActorRespawnSystem.hpp"


#include "../actor/Actor.hpp"       
#include "../actor/Monster.hpp"
#include "../components/ActorRespawnTimer.hpp"
#include "../World.hpp"
#include "../../network/packets/server/status/NpcStatusUpdatePacket.hpp"
using namespace Network::Packet::Server;

void ActorRespawnSystem::updateImpl(ClockDuration elapsed, Actor& actor)
{
    auto timer = actor.component<ActorRespawnTimer>();
    if (!timer)
    {
        return;
    }

    timer->elapsedSinceDeath += elapsed;

    if (timer->elapsedSinceDeath >= timer->respawnDelay)
    {
        if (auto const npc = World::addNpc(timer->npcId); npc && npc->type() == ActorType::Npc)
        {
            npc->setPosition(timer->spawnPosition);

            World::broadcastAround(npc, NpcStatusUpdatePacket(npc));
        }
        else if (auto const monster = World::addMonster(timer->npcId); monster && monster->type() == ActorType::Monster)
        {
            monster->setPosition(timer->spawnPosition);

            World::broadcastAround(monster, NpcStatusUpdatePacket(monster));
        }

        actor.delComponent<ActorRespawnTimer>();
    }
}