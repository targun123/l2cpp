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
    void computeStats();
    void setProfession(Profession profession);

    auto setShortcut(Shortcut shortcut) -> Shortcut &;
    void delShortcut(size_t index);

private:
    struct CharacterImpl;
    Pimpl<CharacterImpl> _impl;
};
