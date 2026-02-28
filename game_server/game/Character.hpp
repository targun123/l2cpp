/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <optional>
#include <string>
#include <vector>

template<typename T>
struct Gauge
{
    T current{};
    T max{current};
};

class Character
{
public:
    u32 id = 1;
    u32 accessLevel = 1;
    std::wstring name = L"test", title = L"{l2cpp}";
    u32 raceId = 0;
    enum class Sex : u32 { Male, Female } sex = Sex::Male;
    u32 classId = 0;
    s32 headAngle = 0;
    u32 active = 1;
    Gauge<double> cp{500};
    Gauge<double> hp{500};
    Gauge<double> mp{500};
    Gauge<u32> weight{0, 10'000};
    u32 sp = 0;
    u32 xp = 0;
    u32 level = 1;
    u32 karma = 0;
    u32 pkCount = 0;
    u32 pvpCount = 0;
    u32 hairStyleId = 0;
    u32 hairColorId = 0;
    u32 faceId = 0;
    u32 deleteTime = 0;
    u32 selected = 1;
    u8 enchantEffect = 0;
    u32 augmentationId = 0;
    u32 transformationId = 0;
    u32 clanId = 0;
    u16 evalAmount = 32, evalScore = 0;
    u16 inventoryLimit = 1000;
    u32 nameColor = 0xFFFFFF; // RGB
    bool isPvpFlagged = false;
    bool isHero = false;
    bool isNoble = false;

    double collisionRadius = 9;
    double collisionHeight = 23;

    std::vector<u16> cubics;

    std::optional<u32> targetId;

    enum class Team : u8 { None, Blue, Red } team = Team::None;

    struct Position
    {
        s32 x = -83968, y = 244634, z = -3500;
    } pos;

    struct Stats
    {
        u32 STR = 40, DEX = 30, CON = 43;
        u32 INT = 21, WIT = 11, MEN = 25;

        u32 pAtk         = 10,  pDef          = 80;
        u32 mAtk         = 6,   mDef          = 40;
        u32 pAtkSpeed    = 300, mAtkSpeed     = 10;
        u32 pAtkRange    = 20,  pAtkRandom    = 10;
        u32 accuracy     = 10,  evasion       = 10;
        u32 pCritRate    = 10,  mCritRate     = 10;
        u32 runSpeed     = 115, walkSpeed     = 80;
        u32 swimRunSpeed = 50,  swimWalkSpeed = 50;
        u32 flyRunSpeed  = 200, flyWalkSpeed  = 150;

        double moveSpeedMutliplier = 1;
        double pAtkSpeedMutliplier = 1;
    } baseStats, finalStats;
};
