/// @author    Chnossos
/// @date      Created on 2026-04-08

#include "SkillCancelPacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"

using Network::Packet::Server::SkillCancelPacket;

SkillCancelPacket::SkillCancelPacket(Actor const & caster)
    : Packet(0x49)
{
    *this
        << caster.id()
    ;
}
