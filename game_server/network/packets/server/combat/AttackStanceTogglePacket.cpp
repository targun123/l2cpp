/// @author    Chnossos
/// @date      Created on 2026-03-21

#include "AttackStanceTogglePacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"

using Network::Packet::Server::AttackStanceTogglePacket;

AttackStanceTogglePacket::AttackStanceTogglePacket(bool const enable, Actor const & target)
    : Packet(enable ? 0x2b : 0x2c)
{
    *this
        << target.id()
    ;
}
