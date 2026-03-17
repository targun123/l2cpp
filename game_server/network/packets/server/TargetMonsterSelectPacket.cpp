/// @author    Chnossos
/// @date      Created on 2026-03-17

#include "TargetMonsterSelectPacket.hpp"

// Project includes
#include "../../../game/actor/Character.hpp"
#include "../../../game/actor/Monster.hpp"

using Network::Packet::Server::TargetMonsterSelectPacket;

TargetMonsterSelectPacket::TargetMonsterSelectPacket(Character const & actor, Monster const & target)
    : Packet(0xa6)
{
    *this
        << target.id()
        << static_cast<u16>(actor.level() - target.level())
    ;
}
