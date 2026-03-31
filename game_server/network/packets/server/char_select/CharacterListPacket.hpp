/// @author    Chnossos
/// @date      Created on 2026-03-31

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Character;
class Player;

namespace Network::Packet::Server { struct CharacterListPacket; }

struct Network::Packet::Server::CharacterListPacket final : public l2cpp::Network::Packet
{
    explicit CharacterListPacket(Player const &, std::vector<Ref<Character>> const & characters);
};
