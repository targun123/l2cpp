/// @author    Chnossos
/// @date      Created on 2026-03-17

#include "TargetSelectPacket.hpp"

// Project includes
#include "../../../game/actor/Character.hpp"

using Network::Packet::Server::TargetSelectPacket;

TargetSelectPacket::TargetSelectPacket(Character const & source, Actor const & target)
    : Packet(0x29)
{
    *this
        << source.id()
        << target.id()
        << source.position().x
        << source.position().y
        << source.position().z
    ;
}
