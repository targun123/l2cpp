/// @author    Chnossos
/// @date      Created on 2026-04-05

#include "TargetClearPacket.hpp"

// Project includes
#include "../../../../game/actor/Character.hpp"
#include "../../../../game/components/Position.hpp"

using Network::Packet::Server::TargetClearPacket;

TargetClearPacket::TargetClearPacket(Character const & c)
    : Packet(0x2a)
{
    *this
        << c.id()
        << c.position()
    ;
}
