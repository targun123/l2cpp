/// @author    Chnossos
/// @date      Created on 2026-03-15

#pragma once

template<typename T>
struct Gauge
{
    T current{};
    T max{current};
};
