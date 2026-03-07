/// @author    Chnossos
/// @date      Created on 2026-03-07

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server
{
    using ClientDisconnect = l2cpp::Network::HeaderOnlyPacket<0x26>;
}
