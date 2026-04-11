/// @author    Chnossos
/// @date      Created on 2026-04-11

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Constants
{
    constexpr byte blowfishToken[] = "_;5.]94-31==-%xT!^[$"; // trailing zero is included by sizeof()
    constexpr auto rsaExponent     = "65537";
    constexpr auto rsaKeyBitSize   = 1024;
}
