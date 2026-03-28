/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

struct MoveAction : public Action
{
    MoveAction() noexcept;

    bool canBeInterrupted() const override;

    void onStarted(Actor &) override;
    void updateImpl(ClockDuration, Actor &) override;

    s32 originX = 0, originY = 0, originZ = 0;
    s32 targetX = 0, targetY = 0, targetZ = 0;

    float currentDistance = 0, totalDistance = 0;

    enum class Input { Keyboard, Mouse } input = Input::Mouse;
};
