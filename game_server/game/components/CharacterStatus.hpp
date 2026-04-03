/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include "ActorStatus.hpp"

#include <l2cpp/Typedefs.hpp>

struct CharacterStatus : public ActorStatus
{
    u32 xp, sp;
    u32 pvpCount, pkCount, karma;

    bool isNoble = false;
    bool isHero  = false;
};
