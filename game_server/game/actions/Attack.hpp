/// @author    Chnossos
/// @date      Created on 2026-03-20

#pragma once

// Project includes
#include "Action.hpp"

template<typename R = std::ratio<1>, typename T> requires std::integral<T> || std::floating_point<T>
auto toClockDuration(T duration) -> std::chrono::steady_clock::duration
{
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<T, R>(duration));
}

struct AttackAction : public Action
{
    explicit AttackAction(u32 const pAtkSpeed) noexcept
        : Action(Type::Attack)
        , hitDuration(toClockDuration(1. / (pAtkSpeed / 500.)))
    {}

    ClockDuration hitDuration;
};
