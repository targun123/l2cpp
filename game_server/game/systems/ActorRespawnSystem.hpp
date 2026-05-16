
#pragma once

// Project includes
#include "../ecs/ActorSystem.hpp"


struct ActorRespawnSystem : public ActorSystem
{
    void updateImpl(ClockDuration elapsed, Actor& actor) override;
};