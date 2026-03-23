/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

template<typename Rep, typename Period>
auto toClockDuration(std::chrono::duration<Rep, Period> duration) -> std::chrono::steady_clock::duration
{
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);
}

struct AttackAction : public Action
{
    explicit AttackAction(u32 const pAtkSpeed) noexcept
        : Action(Type::Attack)
        , hitDuration(toClockDuration(1s / (pAtkSpeed / 500.)))
        , impactTimePoint(toClockDuration(hitDuration * 0.55)) // The impact seems to happen at 55% of anim. (w/o lag)
    {}

    ClockDuration  hitDuration;
    ClockTimePoint impactTimePoint;
};
