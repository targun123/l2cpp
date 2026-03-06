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

using GameObjectId = u32;
using PacketHeader = u16;
using PacketOpCode = u16;

constexpr u8  operator""_u8 (u64 const nbr) { return static_cast<u8> (nbr); }
constexpr u16 operator""_u16(u64 const nbr) { return static_cast<u16>(nbr); }
constexpr u32 operator""_u32(u64 const nbr) { return static_cast<u32>(nbr); }
constexpr u64 operator""_u64(u64 const nbr) { return nbr; }

constexpr s8  operator""_s8 (u64 const nbr) { return static_cast<s8> (nbr); }
constexpr s16 operator""_s16(u64 const nbr) { return static_cast<s16>(nbr); }
constexpr s32 operator""_s32(u64 const nbr) { return static_cast<s32>(nbr); }
constexpr s64 operator""_s64(u64 const nbr) { return static_cast<s64>(nbr); }
