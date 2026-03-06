/// @author    Chnossos
/// @date      Created on 2026-03-04

#pragma once

// Project includes
#include <l2cpp/Typedefs.hpp>

namespace l2
{
    enum class BodyPart : s32
    {
        None,
        Underwear     = 0b00000000'00000000'00000001, ///< 0x1
        RightEar      = 0b00000000'00000000'00000010, ///< 0x2
        LeftEar       = 0b00000000'00000000'00000100, ///< 0x4
        Ears          = RightEar | LeftEar,           ///< 0x6 @warning Not a true slot.
        Neck          = 0b00000000'00000000'00001000, ///< 0x8
        RightFinger   = 0b00000000'00000000'00010000, ///< 0x10
        LeftFinger    = 0b00000000'00000000'00100000, ///< 0x20
        Fingers       = RightFinger | LeftFinger,     ///< 0x30 @warning Not a true slot.
        Head          = 0b00000000'00000000'01000000, ///< 0x40
        RightHand     = 0b00000000'00000000'10000000, ///< 0x80
        LeftHand      = 0b00000000'00000001'00000000, ///< 0x100
        Gloves        = 0b00000000'00000010'00000000, ///< 0x200
        Chest         = 0b00000000'00000100'00000000, ///< 0x400
        Legs          = 0b00000000'00001000'00000000, ///< 0x800
        Feet          = 0b00000000'00010000'00000000, ///< 0x1000
        Back          = 0b00000000'00100000'00000000, ///< 0x2000
        Hands         = 0b00000000'01000000'00000000, ///< 0x4000
        TwoPieceArmor = 0b00000000'10000000'00000000, ///< 0x8000
        Hair          = 0b00000001'00000000'00000000, ///< 0x10000
        Dress         = 0b00000010'00000000'00000000, ///< 0x20000
    };
}
