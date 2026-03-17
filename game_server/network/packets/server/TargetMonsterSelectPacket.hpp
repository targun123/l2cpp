/// @author    Chnossos
/// @date      Created on 2026-03-17

#pragma once

// Project includes
#include <l2cpp/network/Packet.hpp>

class Character;
class Monster;

namespace Network::Packet::Server { struct TargetMonsterSelectPacket; }

struct Network::Packet::Server::TargetMonsterSelectPacket final : public l2cpp::Network::Packet
{
    explicit TargetMonsterSelectPacket(Character const & actor, Monster const & target);
};
