/// @author    Chnossos
/// @date      Created on 2026-03-14

#pragma once

// Project includes
#include <l2cpp/Exception.hpp>

// C++ includes
#include <string_view>

namespace Utils
{
    namespace details
    {
        template<typename T> requires std::integral<T> || std::floating_point<T>
        auto stringViewTo(std::string_view const str) -> T
        {
            T value;
            auto const result = std::from_chars(str.data(), str.data() + str.size(), value);
            L2CPP_B_ASSERT(result.ec == std::errc(), "Failed to convert '{}' into a number", str);
            return value;
        }
    }

    template<typename T> requires std::integral<T> || std::floating_point<T> || std::is_enum_v<T>
    auto stringViewTo(std::string_view const str) -> T
    {
        if constexpr (std::is_enum_v<T>)
            return static_cast<T>(details::stringViewTo<std::underlying_type_t<T>>(str));
        else
            return details::stringViewTo<T>(str);
    }
}
