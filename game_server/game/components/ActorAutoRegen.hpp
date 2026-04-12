/// @author    Chnossos
/// @date      Created on 2026-04-12

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct ActorAutoRegen : public Component
{
    ClockDuration elapsedSinceLastUpdate = ClockDuration::zero();
};
