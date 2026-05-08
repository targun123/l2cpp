/// @author    Chnossos
/// @date      Created on 2026-02-22

#pragma once

// Project includes
#include "Actor.hpp"
#include "../constants/Profession.hpp"

#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

class ItemStorage;
class Player;
class PlayerAppearance;
class Shortcut;
struct CharacterStatus;

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

    bool isAttackable() const override;

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
