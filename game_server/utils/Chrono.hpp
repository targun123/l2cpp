/// @author    Chnossos
/// @date      Created on 2026-03-25

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Utils::Chrono
{
    struct Clock
    {
        std::chrono::steady_clock::time_point start;

        Clock() noexcept: start(std::chrono::steady_clock::now()) {}

        auto restart() -> std::chrono::steady_clock::duration
        {
            auto const d = std::chrono::steady_clock::now() - start;
            start = std::chrono::steady_clock::now();
            return d;
        }

        template<typename Rep, typename Period>
        static constexpr auto toDuration(std::chrono::duration<Rep, Period> d) -> std::chrono::steady_clock::duration
        {
            return std::chrono::duration_cast<std::chrono::steady_clock::duration>(d);
        }
    };

    constexpr bool thresholdCrossed(ClockDuration const totalElapsed,
                                    ClockDuration const elapsed,
                                    ClockDuration const elapsedThreshold)
    {
        return totalElapsed <= elapsedThreshold && totalElapsed + elapsed >= elapsedThreshold;
    }

    constexpr bool thresholdCrossed(ClockTimePoint const currentTimePoint,
                                    ClockDuration  const elapsed,
                                    ClockTimePoint const thresholdTimePoint)
    {
        return currentTimePoint <= thresholdTimePoint && currentTimePoint + elapsed >= thresholdTimePoint;
    }
}
