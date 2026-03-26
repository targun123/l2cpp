/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "systems/ActorAttackStanceTimerSystem.hpp"

#include <l2cpp/network/Packet.hpp>

// C++ includes
#include <ranges>

using l2cpp::Network::Packet;

std::vector<std::unique_ptr<System>>        World::_systems;
std::unordered_map<GameObjectId, Character> World::_characters;
std::unordered_map<GameObjectId, Monster>   World::_monsters;

auto World::characters() -> std::unordered_map<GameObjectId, Character> const &
{
    return _characters;
}

auto World::monsters() -> std::unordered_map<GameObjectId, Monster> const &
{
    return _monsters;
}

auto World::character(GameObjectId const id) -> OptRef<Character>
{
    OptRef<Character> result;

    if (auto const it = _characters.find(id); it != _characters.end())
        result = it->second;

    return result;
}

auto World::monster(GameObjectId const id) -> OptRef<Monster>
{
    OptRef<Monster> result;

    if (auto const it = _monsters.find(id); it != _monsters.end())
        result = it->second;

    return result;
}

void World::init()
{
    registerSystem<ActorAttackStanceTimerSystem>();
}

void World::update(ClockDuration const elapsed)
{
    using namespace std::chrono;

    for (auto & c : _characters | std::views::values)
    {
        if (auto const action = c.currentAction(); action)
            action->update(elapsed, c);
    }

    for (auto const & system : _systems)
    {
        for (auto & c : _characters | std::views::values)
            system->update(elapsed, c);

        for (auto & m : _monsters | std::views::values)
            system->update(elapsed, m);
    }
}

auto World::addCharacter(OptRef<Player> p) -> Character &
{
    Character character(std::move(p));
    auto const & [it, ok] = _characters.try_emplace(character.id(), std::move(character));
    L2CPP_B_ASSERT(ok, "Failed to add a new Character to the world");
    return it->second;
}

void World::delCharacter(GameObjectId const id)
{
    _characters.erase(id);
}

auto World::addMonster() -> Monster &
{
    Monster monster;
    auto const & [it, ok] = _monsters.try_emplace(monster.id(), std::move(monster));
    L2CPP_B_ASSERT(ok, "Failed to add a new Character to the world");
    return it->second;
}

void World::delMonster(GameObjectId const id)
{
    _monsters.erase(id);
}

auto World::inGameTime() -> std::chrono::minutes
{
#ifdef NDEBUG
    constexpr u32 inGameTimeAcceleration = 6; // means 1 in-game day equals 4 hours IRL
    constexpr std::chrono::minutes irlDayInMinutes{60min * 24};
    constexpr std::chrono::minutes inGameDayInMinutes{irlDayInMinutes / inGameTimeAcceleration};

    auto const now          = std::chrono::system_clock::now(); // Server time is always UTC time
    auto const irlTimeOfDay = now - std::chrono::floor<std::chrono::days>(now);
    auto const minutesOfDay = std::chrono::floor<std::chrono::minutes>(irlTimeOfDay);
    auto const percentOfDay = static_cast<double>(minutesOfDay.count()) / irlDayInMinutes.count();
    return std::chrono::floor<std::chrono::minutes>(inGameDayInMinutes * percentOfDay);
#else
    return 60min * 10; // 10am because we want sunlight to see what we're doing
#endif
}

void World::broadcast(Packet && packet)
{
    packet.finalize();
    auto const & p = packet;

    for (auto const & c : _characters | std::views::values)
        c.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
}

void World::broadcastAround(Actor const & emitter, Packet && packet, bool const includeEmitter)
{
    packet.finalize();
    auto const & p = packet;

    for (auto const & c : _characters | std::views::values)
    {
        if (c.id() != emitter.id() || includeEmitter)
            c.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
    }
}
