/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

struct AttackAction : public Action
{
    explicit AttackAction(u32 pAtkSpeed) noexcept;

    bool canBeInterrupted() const override;

    void onStarted(Actor &) override;
    void onFinished(Actor &) override;
    void updateImpl(ClockDuration, Actor &) override;

    ClockDuration  hitDuration;
    ClockTimePoint impactTimePoint;
};
