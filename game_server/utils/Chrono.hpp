/// @author    Chnossos
/// @date      Created on 2026-03-25

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Utils::Chrono
{
    constexpr bool thresholdCrossed(ClockDuration const totalElapsed,
                                    ClockDuration const elapsed,
                                    ClockDuration const elapsedThreshold)
    {
        return totalElapsed < elapsedThreshold && totalElapsed + elapsed >= elapsedThreshold;
    }

    constexpr bool thresholdCrossed(ClockTimePoint const currentTimePoint,
                                    ClockDuration  const elapsed,
                                    ClockTimePoint const thresholdTimePoint)
    {
        return currentTimePoint < thresholdTimePoint && currentTimePoint + elapsed >= thresholdTimePoint;
    }
}
