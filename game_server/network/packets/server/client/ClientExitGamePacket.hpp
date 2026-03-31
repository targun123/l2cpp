/// @author    Chnossos
/// @date      Created on 2026-03-31

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server
{
    using ClientExitGamePacket = l2cpp::Network::HeaderOnlyPacket<0x7E>;
}
