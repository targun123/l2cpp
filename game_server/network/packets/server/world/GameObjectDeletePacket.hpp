/// @author    Chnossos
/// @date      Created on 2026-04-05

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class GameObject;

namespace Network::Packet::Server { struct GameObjectDeletePacket; }

struct Network::Packet::Server::GameObjectDeletePacket final : public l2cpp::Network::Packet
{
    explicit GameObjectDeletePacket(GameObject const &);
};
