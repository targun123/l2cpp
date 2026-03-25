/// @author    Chnossos
/// @date      Created on 2026-03-25

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Utils::Chrono
{
    constexpr bool thresholdCrossed(ClockTimePoint const pointInTime,
                                    ClockDuration  const elapsed,
                                    ClockTimePoint const threshold)
    {
        return pointInTime < threshold && pointInTime + elapsed >= threshold;
    }

    constexpr bool thresholdCrossed(ClockTimePoint const pointInTime,
                                    ClockDuration  const elapsed,
                                    ClockDuration  const thresholdDuration)
    {
        return thresholdCrossed(pointInTime, elapsed, pointInTime + thresholdDuration);
    }
}
