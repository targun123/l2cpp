/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../CompileTimeConfig.hpp"
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "../network/packets/server/world/GameObjectDeletePacket.hpp"
#include "../utils/Maths.hpp"
#include "systems/ActorAttackStanceTimerSystem.hpp"

#include <l2cpp/network/Packet.hpp>

// C++ includes
#include <ranges>

namespace SC = Network::Packet::Server; // Server -> Client

using l2cpp::Network::Packet;

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

    auto & c = addCharacterPreview(L"Admin");
    c.setName(L"test" + std::to_wstring(c.id()));
    c.setTitle(L"{l2cpp}");
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

auto World::getCharacterPreviews(std::wstring_view const playerAccount) -> std::vector<Ref<Character>>
{
    std::vector<Ref<Character>> result;

    auto const & index = _characterPreviewsIndex[playerAccount];
    for (result.reserve(index.size()); auto const id : index)
        result.emplace_back(_characterPreviews.at(id));

    return result;
}

auto World::addCharacterPreview(std::wstring_view const playerAccount) -> Character &
{
    L2CPP_B_ASSERT(!playerAccount.empty(), "Player account name unknown, cannot create character preview");

    Character  c;
    auto const id = c.id();
    _characterPreviewsIndex[playerAccount].emplace_back(id);
    return _characterPreviews.try_emplace(id, std::move(c)).first->second;
}

auto World::loadCharacterFromPreview(Character & c) -> Character &
{
    auto const id  = c.id();
    auto &     ref = _characters.try_emplace(id, std::move(c)).first->second;
    _characterPreviews.erase(id);
    return ref;
}

void World::moveCharacterBackToPreviews(Character & c)
{
    forEachActorAround(c, [&c] (Actor & actor)
    {
        if (actor.type() == ActorType::Character)
        {
            auto const & character = static_cast<Character &>(actor);
            if (character.player)
                character.player->connection().send(SC::GameObjectDeletePacket{c});
        }
    });

    c.player->unsetCurrentCharacter();

    auto const id = c.id();
    _characterPreviews.try_emplace(id, std::move(c));
    _characters.erase(id);
}

auto World::addCharacter(OptRef<Player> p) -> Character &
{
    Character c(std::move(p));
    auto const id = c.id();
    return _characters.try_emplace(id, std::move(c)).first->second;
}

auto World::addMonster() -> Monster &
{
    Monster m;
    auto const id = m.id();
    return _monsters.try_emplace(id, std::move(m)).first->second;
}

void World::delMonster(GameObjectId const id)
{
    _monsters.erase(id);
}

auto World::inGameTime() -> std::chrono::minutes
{
    if constexpr (Config::isDebugMode)
        return 60min * 10; // 10am because we want sunlight to see what we're doing
    else
    {
        constexpr u32                  inGameTimeAcceleration = 6; // means 1 in-game day equals 4 hours IRL
        constexpr std::chrono::minutes irlDayInMinutes{60min * 24};
        constexpr std::chrono::minutes inGameDayInMinutes{irlDayInMinutes / inGameTimeAcceleration};

        auto const now          = std::chrono::system_clock::now(); // Server time is always UTC time
        auto const irlTimeOfDay = now - std::chrono::floor<std::chrono::days>(now);
        auto const minutesOfDay = std::chrono::floor<std::chrono::minutes>(irlTimeOfDay);
        auto const percentOfDay = static_cast<double>(minutesOfDay.count()) / irlDayInMinutes.count();
        return std::chrono::floor<std::chrono::minutes>(inGameDayInMinutes * percentOfDay);
    }
}

void World::subscribeToTarget(Actor const & target, Actor const & listener)
{
    if (target != listener)
        _targetSubscribers[target.id()].emplace_back(listener.id());
}

void World::unsubscribeFromTarget(Actor const & target, Actor const & listener)
{
    _targetSubscribers[target.id()].remove(listener.id());
}

void World::forEachActorAround(Actor const & source, std::function<void(Actor &)> const & f)
{
    if (f)
    {
        auto const distancePred = [&] (Actor const & a) { return isInBroadcastRange(source, a); };
        auto const skipEmitter  = [&] (Actor const & a) { return a != source;                   };

        using namespace std::views;
        auto charactersInRange = _characters | values | filter(distancePred) | filter(skipEmitter);
        auto monstersInRange   = _monsters   | values | filter(distancePred) | filter(skipEmitter);

        for (auto & a : charactersInRange) f(a);
        for (auto & a : monstersInRange)   f(a);
    }
}

bool World::isInBroadcastRange(Actor const & source, Actor const & target)
{
    return Utils::Maths::distance(source, target) <= 1000;
}

void World::broadcast(Packet && packet)
{
    packet.finalize();

    for (auto const & p = packet; auto const & c : _characters | std::views::values)
    {
        if (c.player)
            c.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
    }
}

void World::broadcastAround(Actor const & emitter, Packet && packet, bool const includeEmitter)
{
    packet.finalize();
    auto const & p = packet;

    auto const charHasDriver      = [ ] (Character const & c) { return c.player.has_value();           };
    auto const charIsInRange      = [&] (Character const & c) { return isInBroadcastRange(emitter, c); };
    auto const emitterIfRequested = [&] (Character const & c) { return c != emitter || includeEmitter; };

    using namespace std::views;
    auto view = _characters | values | filter(charHasDriver) | filter(charIsInRange) | filter(emitterIfRequested);

    for (auto const & c : view)
        c.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
}

void World::broadcastToSubscribers(Actor const & emitter, Packet && packet, bool const includeEmitter)
{
    packet.finalize();
    auto const & p = packet;

    for (auto const id : _targetSubscribers[emitter.id()])
    {
        // Ensure we find a player
        if (auto const it = _characters.find(id); it != _characters.end() && it->second.player)
            it->second.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
    }

    if (emitter.type() == ActorType::Character)
    {
        auto const & c = static_cast<Character const &>(emitter);
        if (includeEmitter && c.player)
            c.player->connection().send(Packet(p.opCode()) << p.body().subspan(p.opCode() > 0xff ? 2 : 1));
    }
}

std::vector<std::unique_ptr<System>>                             World::_systems;
std::unordered_map<std::wstring_view, std::vector<GameObjectId>> World::_characterPreviewsIndex;
std::unordered_map<GameObjectId, Character>                      World::_characterPreviews;
std::unordered_map<GameObjectId, Character>                      World::_characters;
std::unordered_map<GameObjectId, Monster>                        World::_monsters;
std::unordered_map<GameObjectId, std::list<GameObjectId>>        World::_targetSubscribers;
