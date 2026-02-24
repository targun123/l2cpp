/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

// C++ includes
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
    u32 sex = 0;
    u32 classId = 0;
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

    double collisionRadius = 20;
    double collisionHeight = 23.5;

    std::vector<u16> cubics;

    enum class Team : u8 { None, Blue, Red } team = Team::None;

    struct
    {
        i32 x = -83968, y = 244634, z = -3500;
    } pos;

    struct
    {
        u32 STR = 10, DEX = 10, CON = 10;
        u32 INT = 10, WIT = 10, MEN = 10;
    } attributes;

    struct
    {
        u32 pAtk          = 10,  pDef         = 10;
        u32 mAtk          = 10,  mDef         = 10;
        u32 pAtkSpeed     = 10,  mAtkSpeed    = 10;
        u32 accuracy      = 10,  evasion      = 10;
        u32 critRate      = 10;
        u32 walkSpeed     = 120, runSpeed     = 180;
        u32 swimWalkSpeed = 80,  swimRunSpeed = 100;
        u32 flyWalkSpeed  = 150, flyRunSpeed  = 200;
    } stats{};
};
