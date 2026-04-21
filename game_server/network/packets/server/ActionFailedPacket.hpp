/// @author    Chnossos
/// @date      Created on 2026-04-21

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace Network::Packet::Server { struct ActionFailedPacket; }

struct Network::Packet::Server::ActionFailedPacket final : public l2cpp::Network::Packet
{
    ActionFailedPacket();
};
