/// @author    Chnossos
/// @date      Created on 2026-03-23

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server
{
    /// Sun rises at 7am IG.
    using SunRisePacket = l2cpp::Network::HeaderOnlyPacket<0x1c>;
}
