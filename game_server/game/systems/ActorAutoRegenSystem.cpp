/// @author    Chnossos
/// @date      Created on 2026-04-12

#include "ActorAutoRegenSystem.hpp"

// Project includes
#include "../World.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../components/ActorAutoRegen.hpp"
#include "../components/Stats.hpp"

/// @returns @c true if rounded-down stat has changed value, else @c false
static bool addRegenTicks(StatValue & stat, StatValue const regenPerSecond, size_t const ticks, StatValue const statMax)
{
    auto const tmp = stat;
    stat = std::min(stat + regenPerSecond * ticks, statMax);
    return stat != tmp;
}

void ActorAutoRegenSystem::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    auto const autoRegen = actor.component<ActorAutoRegen>();
    if (autoRegen && (autoRegen->elapsedSinceLastUpdate += elapsed) >= 3s)
    {
        // A lot of time could have elapsed since last update, account for that
        size_t const ticks = std::chrono::floor<std::chrono::seconds>(autoRegen->elapsedSinceLastUpdate).count();

        autoRegen->elapsedSinceLastUpdate %= 3s;

        std::unordered_map<Stat, StatValue> updates;

        using enum StatId;

        auto & stats = *actor.component<Stats>();
        if (stats[CurHp] < stats[MaxHp] && addRegenTicks(stats[CurHp], stats[HpRegen], ticks, stats[MaxHp]))
            updates.try_emplace(Stat::CurHp, stats[CurHp]);

        if (stats[CurMp] < stats[MaxMp] && addRegenTicks(stats[CurMp], stats[HpRegen], ticks, stats[MaxMp]))
            updates.try_emplace(Stat::CurMp, stats[CurMp]);

        if (stats[CurCp] < stats[MaxCp] && addRegenTicks(stats[CurCp], stats[HpRegen], ticks, stats[MaxCp]))
            updates.try_emplace(Stat::CurCp, stats[CurCp]);

        if (!updates.empty())
        {
            Network::Packet::Server::StatsUpdatePacket p(actor);

            for (auto const [stat, value] : updates)
                p.addStat(stat, value);

            World::broadcastToSubscribers(actor, std::move(p), true);
        }
    }
}
