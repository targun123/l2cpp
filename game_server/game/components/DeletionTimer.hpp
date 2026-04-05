/// @author    Chnossos
/// @date      Created on 2026-04-05

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct DeletionTimer : public Component
{
    ClockDuration timeBeforeDeletion;
    ClockDuration elapsedSinceDeath;
};
