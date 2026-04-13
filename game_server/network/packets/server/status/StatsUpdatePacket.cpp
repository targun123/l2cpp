/// @author    Chnossos
/// @date      Created on 2026-04-02

#include "StatsUpdatePacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"

using Network::Packet::Server::StatsUpdatePacket;

StatsUpdatePacket::StatsUpdatePacket(Actor const & emitter)
    : Packet(0x0e, "StatsUpdate")
{
    *this
        << emitter.id()
    ;
}

StatsUpdatePacket & StatsUpdatePacket::addStat(Stat const id, s32 const value)
{
    _statValues[id] = value;
    return *this;
}

void StatsUpdatePacket::finalizeImpl()
{
    *this
        << static_cast<u32>(_statValues.size())
    ;

    for (auto const [id, value] : _statValues)
        *this << id << value;
}
