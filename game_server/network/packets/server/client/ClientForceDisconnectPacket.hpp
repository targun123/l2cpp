/// @author    Chnossos
/// @date      Created on 2026-03-07

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct ClientForceDisconnectPacket; }

struct Network::Packet::Server::ClientForceDisconnectPacket final : public l2cpp::Network::Packet
{
    ClientForceDisconnectPacket();
};
