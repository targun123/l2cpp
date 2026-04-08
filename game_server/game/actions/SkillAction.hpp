/// @author    Chnossos
/// @date      Created on 2026-04-08

#pragma once

// Project includes
#include "Action.hpp"

#include <l2cpp/Pimpl.hpp>

class Skill;

class SkillAction final : public Action
{
public:
    explicit SkillAction(Skill & skill);
    SkillAction(SkillAction &&) noexcept;
    SkillAction & operator=(SkillAction &&) noexcept;

public:
    bool canBeInterrupted() const override;

private:
    void onStarted(Actor &) override;
    void updateImpl(ClockDuration, Actor &) override;
    void onFinished(Actor &) override;

private:
    struct SkillActionImpl;
    Pimpl<SkillActionImpl> _impl;
};
