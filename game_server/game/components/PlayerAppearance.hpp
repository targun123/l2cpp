/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "../constants/Race.hpp"
#include "../constants/Sex.hpp"
#include "ActorAppearance.hpp"

struct PlayerAppearance : public ActorAppearance
{
    Race race        = Race::Human;
    Sex  sex         = Sex::Male;
    u32  hairStyleId = 0;
    u32  hairColorId = 0;
    u32  faceId      = 0;
    u32  nameColor   = 0xffffffff;
};
