/// @author    Chnossos
/// @date      Created on 2026-04-11

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace Constants
{
    constexpr auto host = "127.0.0.1"sv;
    constexpr auto port = 2106;

    constexpr u32  protocol        = 0xc621;
    constexpr byte blowfishToken[] = "_;5.]94-31==-%xT!^[$"; // trailing zero is included by sizeof()
    constexpr auto rsaExponent     = "65537";
    constexpr auto rsaKeyBitSize   = 1024;
}
