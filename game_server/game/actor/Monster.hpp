/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "NonPlayableActor.hpp"

#include <l2cpp/Pimpl.hpp>

/// Enter brief here.
class Monster : public NonPlayableActor
{
public:
    Monster();
    Monster(Monster &&) noexcept;
    Monster & operator=(Monster &&) noexcept;
    ~Monster() override;

public:
    bool isAttackable() const override;
    auto level() const -> u32;

public:
    void setLevel(u32 level);

private:
    struct MonsterActorImpl;
    Pimpl<MonsterActorImpl> _impl;
};
