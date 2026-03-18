/// @author    Chnossos
/// @date      Created on 2026-02-16

#pragma once

#include <l2cpp/Typedefs.hpp>

/// Packet types received from client
enum class RecvPacket : u8
{
    Authentication      = 0x00,
    GameServerSelection = 0x02,
    GameServerList      = 0x05,
    GameGuard           = 0x07,
};

inline bool operator==(RecvPacket const p, byte const type)
{
    return static_cast<byte>(p) == type;
}

/// Packet types sent to client
enum class SentPacket : u8
{
    Initialization               = 0x00,
    AuthenticationFailed         = 0x01,
    AccountIsBlocked             = 0x02,
    AuthenticationSuccess        = 0x03,
    GameServerList               = 0x04,
    GameServerSelectionFailure   = 0x06,
    GameServerSelectionSuccess   = 0x07,
    GameGuard                    = 0x0b,
};

inline bool operator==(SentPacket const p, byte const type)
{
    return static_cast<byte>(p) == type;
}
