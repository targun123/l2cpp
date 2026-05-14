/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include "../../Typedefs.hpp"
#include "../constants/Profession.hpp"
#include "../constants/Ui.hpp"
#include "Actor.hpp"

#include <l2cpp/Pimpl.hpp>

// C++ includes
#include <span>

class CharacterStatus;
class ItemStorage;
class Player;
class PlayerAppearance;
class Shortcut;

class Character : public Actor
{
public:
    OptRef<Player> player;

public:
    explicit Character(OptRef<Player> = {});
    Character(Character &&) noexcept;
    Character & operator=(Character &&) noexcept;
    ~Character() override;

public:
    u32 accessLevel = 1;

public:
    auto profession() const -> Profession;

    auto status()       -> CharacterStatus       &;
    auto status() const -> CharacterStatus const &;

    auto appearance()       -> PlayerAppearance       &;
    auto appearance() const -> PlayerAppearance const &;

    auto inventory()       -> ItemStorage       &;
    auto inventory() const -> ItemStorage const &;

    auto shortcuts() const -> std::span<Shortcut const, Constants::maxShortcuts>;

    bool isAttackable() const override { return false; }

public:
    void setProfession(Profession);

    auto setShortcut(Shortcut shortcut) -> Shortcut &;
    void delShortcut(size_t index);

    void offerResurrection(Actor const & emitter);
    void answerConfirmationModal(u32 systemMessageId, bool accepted);

private:
    struct CharacterImpl;
    Pimpl<CharacterImpl> _impl;
};
