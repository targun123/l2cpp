/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include "Actor.hpp"
#include "../Gauge.hpp"
#include "../constants/Profession.hpp"

#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <optional>
#include <vector>

class Gear;
class ItemStorage;
class PlayerAppearance;
class Shortcut;
class SkillDirectory;

class Character : public Actor
{
public:
    Character();
    Character(Character &&) noexcept;
    Character & operator=(Character &&) noexcept;
    ~Character() override;

public:
    u32 accessLevel = 1;
    Gauge<double> cp{500};
    Gauge<double> hp{500};
    Gauge<double> mp{500};
    Gauge<u32> weight{0, 10'000};
    u32 deleteTime = 0;
    u32 selected = 1;
    u16 evalAmount = 32, evalScore = 0;

    std::vector<u16> cubics;

    std::optional<u32> targetId;

    struct Stats
    {
        u32 STR = 40, DEX = 30, CON = 43;
        u32 INT = 21, WIT = 11, MEN = 25;

        u32 pAtk         = 10,  pDef          = 80;
        u32 mAtk         = 6,   mDef          = 40;
        u32 pAtkSpeed    = 300, mAtkSpeed     = 333;
        u32 pAtkRange    = 20,  pAtkRandom    = 10;
        u32 accuracy     = 10,  evasion       = 10;
        u32 pCritRate    = 10,  mCritRate     = 10;
        u32 runSpeed     = 115, walkSpeed     = 80;
        u32 swimRunSpeed = 50,  swimWalkSpeed = 50;
        u32 flyRunSpeed  = 0,   flyWalkSpeed  = 0;

        double moveSpeedMutliplier = 1;
        double pAtkSpeedMutliplier = 1;
    } baseStats, finalStats;

public:
    auto appearance()       -> PlayerAppearance       &;
    auto appearance() const -> PlayerAppearance const &;
    auto profession() const -> Profession;

    auto inventory()       -> ItemStorage       &;
    auto inventory() const -> ItemStorage const &;

    auto gear()       -> Gear       &;
    auto gear() const -> Gear const &;

    auto skills()       -> SkillDirectory &;
    auto skills() const -> SkillDirectory const &;

public:
    void setProfession(Profession profession);

    auto setShortcut(Shortcut shortcut) -> Shortcut &;
    void delShortcut(size_t index);

private:
    struct CharacterImpl;
    Pimpl<CharacterImpl> _impl;
};
