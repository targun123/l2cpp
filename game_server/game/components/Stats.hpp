/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "../ecs/Component.hpp"

#include <l2cpp/Typedefs.hpp>

class Actor;

struct Stats : public Component
{
    void compute(Actor const &);

#define CORE_STAT(stat)   StatValue stat = 0, base##stat = 0, bonus##stat = 0
#define STAT(stat)        StatValue stat = 0, stat##Base = 0, stat##Bonus = 0; double stat##Multiplier = 1
#define DOUBLE_STAT(stat) double    stat = 0, stat##Base = 0, stat##Bonus = 0,        stat##Multiplier = 1
#define SPEED_STAT(stat)  StatValue stat = 0, stat##Base = 0

    double curHp = 0; DOUBLE_STAT(maxHp); DOUBLE_STAT(hpRegen);
    double curMp = 0; DOUBLE_STAT(maxMp); DOUBLE_STAT(mpRegen);
    double curCp = 0; DOUBLE_STAT(maxCp); DOUBLE_STAT(cpRegen);

    CORE_STAT(STR); CORE_STAT(DEX); CORE_STAT(CON);
    CORE_STAT(INT); CORE_STAT(WIT); CORE_STAT(MEN);

    STAT(pAtk);      STAT(pDef);
    STAT(mAtk);      STAT(mDef);
    STAT(pAtkSpeed); STAT(mAtkSpeed);
    STAT(pAtkRange); STAT(pAtkRandom);
    STAT(accuracy);  STAT(evasion);
    STAT(pCritRate); STAT(mCritRate);

    STAT(moveSpeed);
    SPEED_STAT(runSpeed);     SPEED_STAT(walkSpeed);
    SPEED_STAT(swimRunSpeed); SPEED_STAT(swimWalkSpeed);
    SPEED_STAT(flyRunSpeed);  SPEED_STAT(flyWalkSpeed);

    StatValue curWeight = 0; STAT(maxWeight);

    STAT(inventoryLimit);
    STAT(wharehouseLimit);
    STAT(freightLimit);
    STAT(storeLimit);
    STAT(commonCraftLimit);
    STAT(dwarvenCraftLimit);

#undef CORE_STAT
#undef REGEN_STAT
#undef SPEED_STAT
#undef STAT
};
