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
    explicit SkillAction(Actor & performer, Skill & skill);
    SkillAction(SkillAction &&) noexcept;
    SkillAction & operator=(SkillAction &&) noexcept;

public:
    bool canBeInterruptedByAnotherAction() const override;

private:
    void onStarted() override;
    void updateImpl(ClockDuration) override;
    void onFinished() override;

private:
    struct SkillActionImpl;
    Pimpl<SkillActionImpl> _impl;
};
