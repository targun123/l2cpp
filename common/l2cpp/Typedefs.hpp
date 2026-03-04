/// @author    Chnossos
/// @date      Created on 2026-02-16

#pragma once

// C++ includes
#include <cstdint>
#include <optional>

using byte = std::uint8_t;
using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;

using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

template<typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;

using PacketHeader = u16;
using PacketOpCode = u16;
