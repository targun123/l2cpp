/// @author    Chnossos
/// @date      Created on 2026-02-28

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Character;

namespace Network::Packet::Server { class CharacterStatusUpdateBroadcastPacket; }

class Network::Packet::Server::CharacterStatusUpdateBroadcastPacket final : public l2cpp::Network::Packet
{
public:
    explicit CharacterStatusUpdateBroadcastPacket(Character const &);
};
