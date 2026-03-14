/// @author    Chnossos
/// @date      Created on 2026-03-14

#pragma once

// Project includes
#include <l2cpp/Exception.hpp>

// C++ includes
#include <string_view>

namespace Utils
{
    template<typename T>
    requires (std::integral<T> || std::floating_point<T>)
    auto stringViewTo(std::string_view str) -> T
    {
        T value;
        auto const result = std::from_chars(str.data(), str.data() + str.size(), value);
        L2CPP_B_ASSERT(result.ec == std::errc(), "Failed to convert '{}' into a number", str);
        return value;
    }
}
