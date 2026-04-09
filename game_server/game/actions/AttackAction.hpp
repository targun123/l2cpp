/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

class AttackAction : public Action
{
public:
    AttackAction(Actor & performer, Actor & target, u32 pAtkSpeed) noexcept;

public:
    bool canBeInterruptedByAnotherAction() const override;

private:
    void onStarted() override;
    void updateImpl(ClockDuration) override;
    void onFinished() override;

private:
    Actor &        _target;
    ClockDuration  _hitDuration;
};
