/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include "ActorStatus.hpp"

#include <l2cpp/Typedefs.hpp>

struct CharacterStatus : public ActorStatus
{
    u32 xp = 0, sp = 0;
    u32 pvpCount = 0, pkCount = 0, karma = 0;

    bool isNoble = false;
    bool isHero  = false;
};
