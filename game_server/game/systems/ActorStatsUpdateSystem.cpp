/// @author    Chnossos
/// @date      Created on 2026-05-04

#include "ActorStatsUpdateSystem.hpp"

// Project includes
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../World.hpp"
#include "../actor/Character.hpp"
#include "../actor/Npc.hpp"
#include "../components/CharacterStatus.hpp"
#include "../components/Stats.hpp"

// Third-party includes
#include <spdlog/spdlog.h>

// C++ includes
#include <unordered_set>

namespace SC = Network::Packet::Server;

void ActorStatsUpdateSystem::updateImpl(ClockDuration, Actor & actor)
{
    static std::unordered_map<GameObjectId, Stats>           statSnapshots;
    static std::unordered_map<GameObjectId, CharacterStatus> statusSnapshots;

    if (auto const it = statSnapshots.find(actor.id()); it != statSnapshots.end())
    {
        if (actor.type() == ActorType::Character)
            updateCharacterStats(static_cast<Character &>(actor), it->second);
        else
            updateNpcStats(static_cast<Npc &>(actor), it->second);
    }

    // Take a snapshot for the next update
    statSnapshots.insert_or_assign(actor.id(), actor.stats());
    if (actor.type() == ActorType::Character)
        statusSnapshots.insert_or_assign(actor.id(), static_cast<Character const &>(actor).status());
}

void ActorStatsUpdateSystem::updateCharacterStats(Character & c, Stats const & oldStats) const
{
    auto const & newStats = c.stats();

    SC::StatsUpdatePacket privatePacket(c);
    for (size_t i = 0; i < std::to_underlying(StatId::Count); ++i)
    {
        if (oldStats[i] != newStats[i])
        {
            auto const statId = static_cast<StatId>(i);
            privatePacket.addStat(statId, newStats[i]);
        }
    }

    if (!privatePacket.empty())
        World::send(c, std::move(privatePacket));

    if (newStats[StatId::CurHp] == 0 && oldStats[StatId::CurHp] > 0)
        c.die();
}

void ActorStatsUpdateSystem::updateNpcStats(Npc & npc, Stats const & oldStats) const
{
    static std::unordered_set publicStats { StatId::CurHp, StatId::MaxHp };

    auto const & newStats = npc.stats();

    SC::StatsUpdatePacket publicPacket(npc);
    for (size_t i = 0; i < std::to_underlying(StatId::Count); ++i)
    {
        if (oldStats[i] != newStats[i])
        {
            auto const statId = static_cast<StatId>(i);
            if (publicStats.contains(statId))
                publicPacket.addStat(statId, newStats[i]);
        }
    }

    if (!publicPacket.empty())
        World::broadcastToSubscribers(npc, std::move(publicPacket));

    if (newStats[StatId::CurHp] == 0 && oldStats[StatId::CurHp] > 0)
        npc.die();
}
