/// @author    Chnossos
/// @date      Created on 2026-04-02

#include "StatsUpdatePacket.hpp"

// Project includes
#include "../../../../game/actor/Actor.hpp"

using Network::Packet::Server::StatsUpdatePacket;

StatsUpdatePacket::StatsUpdatePacket(Actor const & emitter)
    : Packet(0x0e, "StatsUpdate")
{
    *this << emitter.id();

    appendCounterAndStoreOffset(_statsCountOffset);
}

auto StatsUpdatePacket::addStat(Stat const id, s32 const value) -> Packet &
{
    counterAtOffset(_statsCountOffset) += 1;
    return *this << id << value;
}
