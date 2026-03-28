/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

class AttackAction : public Action
{
public:
    AttackAction(Actor & target, u32 pAtkSpeed) noexcept;

public:
    bool canBeInterrupted() const override;

private:
    void onStarted(Actor &) override;
    void updateImpl(ClockDuration, Actor &) override;
    void onFinished(Actor &) override;

private:
    Actor &        _target;
    ClockDuration  _hitDuration;
};
