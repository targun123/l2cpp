
#pragma once
#include "../ecs/Component.hpp"
#include <l2cpp/Typedefs.hpp>
#include "../components/Position.hpp"

struct ActorRespawnTimer : public Component {
    u32 npcId; 
    ClockDuration respawnDelay;
    ClockDuration elapsedSinceDeath{ ClockDuration::zero() };
    Position spawnPosition;
};