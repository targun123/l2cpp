/// @author    Chnossos
/// @date      Created on 2026-05-04

#pragma once

// Project includes
#include "../ecs/ActorSystem.hpp"

namespace Network::Packet::Server { class StatsUpdatePacket; }

class Character;
struct CharacterStatus;
class Npc;

struct ActorStatsUpdateSystem : public ActorSystem
{
    void updateImpl(ClockDuration elapsed, Actor &) override;

private:
    void updateCharacterStats(Network::Packet::Server::StatsUpdatePacket & privatePacket,
                              Network::Packet::Server::StatsUpdatePacket & publicPacket,
                              Character &, Stats const & oldStats) const;

    void updateCharacterStatus(Network::Packet::Server::StatsUpdatePacket & privatePacket,
                               Network::Packet::Server::StatsUpdatePacket & publicPacket,
                               Character &, CharacterStatus const & oldStatus) const;

    void updateNpcStats(Npc &, Stats const & oldStats) const;
};
