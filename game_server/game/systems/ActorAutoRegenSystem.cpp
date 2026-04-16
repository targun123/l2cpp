/// @author    Chnossos
/// @date      Created on 2026-04-12

#include "ActorAutoRegenSystem.hpp"

// Project includes
#include "../World.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../components/ActorAutoRegen.hpp"
#include "../components/Stats.hpp"

/// @returns @c true if rounded-down stat has changed value, else @c false
static bool addRegenTicks(double & stat, double const regenPerSecond, size_t const ticks, double const statMax)
{
    auto const tmp = static_cast<StatValue>(stat);
    stat = std::min(stat + regenPerSecond * ticks, statMax);
    return static_cast<StatValue>(stat) != tmp;
}

void ActorAutoRegenSystem::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    auto const autoRegen = actor.component<ActorAutoRegen>();
    if (autoRegen && (autoRegen->elapsedSinceLastUpdate += elapsed) >= 3s)
    {
        // A lot of time could have elapsed since last update, account for that
        size_t const ticks = std::chrono::floor<std::chrono::seconds>(autoRegen->elapsedSinceLastUpdate).count();

        autoRegen->elapsedSinceLastUpdate %= 3s;

        std::unordered_map<Stat, double> updates;

        auto & stats = *actor.component<Stats>();
        if (stats.curHp < stats.maxHp && addRegenTicks(stats.curHp, stats.hpRegen, ticks, stats.maxHp))
            updates.try_emplace(Stat::CurHp, stats.curHp);

        if (stats.curMp < stats.maxMp && addRegenTicks(stats.curMp, stats.hpRegen, ticks, stats.maxMp))
            updates.try_emplace(Stat::CurMp, stats.curMp);

        if (stats.curCp < stats.maxCp && addRegenTicks(stats.curCp, stats.hpRegen, ticks, stats.maxCp))
            updates.try_emplace(Stat::CurCp, stats.curCp);

        if (!updates.empty())
        {
            Network::Packet::Server::StatsUpdatePacket p(actor);

            for (auto const [stat, value] : updates)
                p.addStat(stat, value);

            World::broadcastToSubscribers(actor, std::move(p), true);
        }
    }
}
