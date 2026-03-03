/// @author    Chnossos
/// @date      Created on 2026-02-28

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

namespace l2 { class Character; }

namespace Network::Packet::Server { class EntityStatusUpdatePacket; }

class Network::Packet::Server::EntityStatusUpdatePacket final : public l2cpp::Network::Packet
{
public:
    explicit EntityStatusUpdatePacket(l2::Character &);
};
