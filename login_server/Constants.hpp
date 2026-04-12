/// @author    Chnossos
/// @date      Created on 2026-04-11

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Constants
{
    constexpr inline auto host = "127.0.0.1"sv;
    constexpr inline auto port = 2106;

    constexpr inline u32  protocol        = 0xc621;
    constexpr inline byte blowfishToken[] = "_;5.]94-31==-%xT!^[$"; // trailing zero is included by sizeof()
    constexpr inline auto rsaExponent     = "65537";
    constexpr inline auto rsaKeyBitSize   = 1024;
}
