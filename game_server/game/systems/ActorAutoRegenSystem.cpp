/// @author    Chnossos
/// @date      Created on 2026-04-12

#include "ActorAutoRegenSystem.hpp"

// Project includes
#include "../World.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../components/ActorAutoRegen.hpp"
#include "../components/Stats.hpp"

void ActorAutoRegenSystem::updateImpl(ClockDuration const elapsed, Actor & actor)
{
    auto const autoRegen = actor.component<ActorAutoRegen>();
    if (autoRegen && (autoRegen->elapsedSinceLastUpdate += elapsed) >= 1s)
    {
        // A lot of time could have elapsed since last update, account for that
        auto const ticks = std::chrono::floor<std::chrono::seconds>(autoRegen->elapsedSinceLastUpdate).count();

        autoRegen->elapsedSinceLastUpdate %= 1s;

        auto & stats = *actor.component<ComputedStats>();

        std::unordered_map<Stat, double> updates;
        if (stats.curHp < stats.maxHp)
        {
            stats.curHp = std::min(stats.curHp + stats.hpRegen * ticks, stats.maxHp);
            updates.try_emplace(Stat::CurHp, stats.curHp);
        }

        if (stats.curMp < stats.maxMp)
        {
            stats.curMp = std::min(stats.curMp + stats.mpRegen * ticks, stats.maxMp);
            updates.try_emplace(Stat::CurMp, stats.curMp);
        }

        if (stats.curCp < stats.maxCp)
        {
            stats.curCp = std::min(stats.curCp + stats.cpRegen * ticks, stats.maxCp);
            updates.try_emplace(Stat::CurCp, stats.curCp);
        }

        if (!updates.empty())
        {
            Network::Packet::Server::StatsUpdatePacket p(actor);

            for (auto const [stat, value] : updates)
                p.addStat(stat, value);

            World::broadcastToSubscribers(actor, std::move(p), true);
        }
    }
}
