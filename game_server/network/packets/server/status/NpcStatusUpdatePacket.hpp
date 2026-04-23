/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Npc;

namespace Network::Packet::Server { struct NpcStatusUpdatePacket; }

struct Network::Packet::Server::NpcStatusUpdatePacket final : public l2cpp::Network::Packet
{
    explicit NpcStatusUpdatePacket(Npc const &);
};
