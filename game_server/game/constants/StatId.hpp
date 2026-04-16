/// @author    Chnossos
/// @date      Created on 2026-04-16

#pragma once

#define CORE_STAT(name)  name, Base##name, Bonus##name
#define STAT(name)       name, Base##name, name##Multiplier, Bonus##name
#define SPEED_STAT(name) name, Base##name

enum class StatId
{
    CORE_STAT(Str), CORE_STAT(Dex), CORE_STAT(Con),
    CORE_STAT(Int), CORE_STAT(Wit), CORE_STAT(Men),

    STAT(PAtk),      STAT(PDef),
    STAT(MAtk),      STAT(MDef),
    STAT(PAtkSpeed), STAT(MAtkSpeed),
    STAT(PAtkRange), STAT(PAtkRandom),
    STAT(Accuracy),  STAT(Evasion),
    STAT(PCritRate), STAT(MCritRate),

    STAT(MoveSpeed),
    SPEED_STAT(runSpeed),     SPEED_STAT(walkSpeed),
    SPEED_STAT(swimRunSpeed), SPEED_STAT(swimWalkSpeed),
    SPEED_STAT(flyRunSpeed),  SPEED_STAT(flyWalkSpeed),

    CurHp,  STAT(MaxHp), STAT(HpRegen),
    CurMp,  STAT(MaxMp), STAT(MpRegen),
    CurCp,  STAT(MaxCp), STAT(CpRegen),
    Weight, STAT(MaxWeight),

    CORE_STAT(InventoryLimit),
    CORE_STAT(WharehouseLimit),
    CORE_STAT(FreightLimit),
    CORE_STAT(StoreLimit),
    CORE_STAT(CommonCraftLimit),
    CORE_STAT(DwarvenCraftLimit),
};

#undef SPEED_STAT
#undef STAT
#undef CORE_STAT
