/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

struct Stats : public Component
{
    u32 STR = 0, DEX = 0, CON = 0;
    u32 INT = 0, WIT = 0, MEN = 0;

    u32 pAtk         = 0, pDef          = 0;
    u32 mAtk         = 0, mDef          = 0;
    u32 pAtkSpeed    = 0, mAtkSpeed     = 0;
    u32 pAtkRange    = 0, pAtkRandom    = 0;
    u32 accuracy     = 0, evasion       = 0;
    u32 pCritRate    = 0, mCritRate     = 0;
    u32 runSpeed     = 0, walkSpeed     = 0;
    u32 swimRunSpeed = 0, swimWalkSpeed = 0;
    u32 flyRunSpeed  = 0, flyWalkSpeed  = 0; ///< Unused
};

struct ComputedStats : public Stats
{
    explicit ComputedStats(Stats stats) noexcept
        : Stats(std::move(stats))
    {}

    double moveSpeedMutliplier = 1;
    double pAtkSpeedMutliplier = 1;
};
