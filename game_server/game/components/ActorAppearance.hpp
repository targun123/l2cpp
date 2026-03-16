/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct ActorAppearance : public Component
{
    double collisionRadius = 0;
    double collisionHeight = 0;
    s32    headAngle       = 0;

    ~ActorAppearance() override = 0;
};

inline ActorAppearance::~ActorAppearance() = default;
