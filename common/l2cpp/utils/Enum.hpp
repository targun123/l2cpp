/// @author    Chnossos
/// @date      Created on 2026-03-05

#pragma once

// C++ includes
#include <concepts>
#include <type_traits>
#include <utility>

namespace l2cpp::utils
{
    template<typename E, typename Func> requires std::is_enum_v<E> && std::invocable<Func, E>
    constexpr void foreach(E begin, E end, Func f)
    {
        for (auto b = std::to_underlying(begin), e = std::to_underlying(end); b < e; ++b)
            f(static_cast<E>(b));
    }

    template<typename E> requires std::is_enum_v<E>
    constexpr bool isInContiguousRange(E index, E start, E end)
    {
        return std::to_underlying(index) >= std::to_underlying(start)
            && std::to_underlying(index)  < std::to_underlying(end);
    }
}
