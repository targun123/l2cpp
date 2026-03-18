/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include "ActorStatus.hpp"

#include <l2cpp/Typedefs.hpp>

struct CharacterStatus : public ActorStatus
{
    Gauge<double> cp{500};
    Gauge<double> mp{500};
    Gauge<u32>    weight{0, 10'000};

    double cpRegen = 1; ///< Per second.
    double mpRegen = 1; ///< Per second.

    u32 xp, sp;
    u32 pvpCount, pkCount, karma;

    bool isNoble = false;
    bool isHero  = false;
};
