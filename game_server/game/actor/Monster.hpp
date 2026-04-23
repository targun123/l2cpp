/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Npc.hpp"

#include <l2cpp/Pimpl.hpp>

class Monster : public Npc
{
public:
    Monster() noexcept;
    Monster(Monster &&) noexcept;
    Monster & operator=(Monster &&) noexcept;
    ~Monster() override;

public:
    bool isAttackable() const override;

private:
    struct MonsterActorImpl;
    Pimpl<MonsterActorImpl> _impl;
};
