/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "../network/packets/server/target/TargetClearPacket.hpp"
#include "../network/packets/server/world/GameObjectDeletePacket.hpp"
#include "../utils/Maths.hpp"
#include "components/DeletionTimer.hpp"
#include "systems/ActorAttackStanceTimerSystem.hpp"
#include "systems/ActorAutoRegenSystem.hpp"
#include "systems/ActorDeletionTimerSystem.hpp"
#include "systems/ActorSkillEffectSystem.hpp"

#include <l2cpp/CompileTimeConfig.hpp>
#include <l2cpp/network/Packet.hpp>

// Third-party
#include <spdlog/spdlog.h>

// C++ includes
#include <ranges>

namespace SC = Network::Packet::Server; // Server -> Client

using l2cpp::Network::Packet;

auto World::actor(GameObjectId const id) -> OptRef<Actor>
{
    /**/ if (auto const c = character(id)) return c;
    else if (auto const m = monster(id))   return m;
    else                                   return std::nullopt;
}

auto World::character(GameObjectId const id) -> OptRef<Character>
{
    auto const it = _characters.find(id);
    return it != _characters.end() ? OptRef(it->second) : std::nullopt;
}

auto World::monster(GameObjectId const id) -> OptRef<Monster>
{
    auto const it = _monsters.find(id);
    return it != _monsters.end() ? OptRef(it->second) : std::nullopt;
}

void World::init()
{
    registerSystem<ActorAttackStanceTimerSystem>();
    registerSystem<ActorAutoRegenSystem>();
    registerSystem<ActorDeletionTimerSystem>();
    registerSystem<ActorSkillEffectSystem>();

    auto & c = addCharacterPreview(L"Admin");
    c.setName(L"test" + std::to_wstring(c.id()));
    c.setTitle(L"{l2cpp}");
}

void World::update(ClockDuration const elapsed)
{
    using namespace std::chrono;

    for (auto & c : _characters | std::views::values)
    {
        try
        {
            if (auto const action = c.currentAction())
                action->update(elapsed);
        }
        catch (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Failed to update action for character '{}':\n{}", c.id(), l2cpp::formatExceptionStack(e));
        }
    }

    for (auto const & system : _systems)
    {
        for (auto & c : _characters | std::views::values) system->update(elapsed, c);
        for (auto & m : _monsters   | std::views::values) system->update(elapsed, m);
    }

    // Death is handled outside the system loops because internals are modified upon death
    for (auto & c : _characters | std::views::values) if (c.dying()) c.die();
    for (auto & m : _monsters   | std::views::values) if (m.dying()) m.die();

    for (Actor & a : _scheduledForDeletion | std::views::values)
        delActor(a);

    _scheduledForDeletion.clear();
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
    if (auto const target = c.target())
    {
        unsubscribeFromTarget(target, c);
        c.setTarget(std::nullopt);
    }

    unsubscribeAllTargetListeners(c);
    broadcastAround(c, SC::GameObjectDeletePacket{c});

    if (c.player)
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

void World::scheduleForDeletion(Actor & a, ClockDuration const timeFromNow)
{
    if (timeFromNow > ClockDuration::zero())
    {
        auto & timer = a.getOrAddComponent<DeletionTimer>();
        timer.timeBeforeDeletion = timeFromNow;
        timer.elapsedSinceDeath  = ClockDuration::zero();
    }
    else
        _scheduledForDeletion.try_emplace(a.id(), a);
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

void World::unsubscribeAllTargetListeners(Actor const & target)
{
    // ReSharper disable once CppLocalVariableMayBeConst
    auto v = _targetSubscribers[target.id()]
           | std::views::transform([] (GameObjectId const id) -> Ref<Character> { return character(id); })
           | std::views::filter([] (Character const & c) { return c.player.has_value(); });

    for (Character & c : v)
    {
        c.setTarget(std::nullopt);
        c.player->connection().send(SC::TargetClearPacket{c});
    }

    _targetSubscribers[target.id()].clear();
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

void World::send(Actor const & to, Packet & packet, std::source_location const & src)
{
    if (to.type() == ActorType::Character)
    {
        if (auto const & c = static_cast<Character const &>(to); c.player)
            c.player->connection().send(packet, src);
    }
}

void World::broadcast(Packet && packet, std::source_location const & src)
{
    for (auto const & c : _characters | std::views::values)
    {
        if (c.player)
            c.player->connection().send(Packet(packet), src);
    }
}

void World::broadcastAround(Actor const & emitter, Packet && packet, bool const includeEmitter,
                            std::source_location const & src)
{
    auto const charHasDriver      = [ ] (Character const & c) { return c.player.has_value();           };
    auto const charIsInRange      = [&] (Character const & c) { return isInBroadcastRange(emitter, c); };
    auto const emitterIfRequested = [&] (Character const & c) { return c != emitter || includeEmitter; };

    using namespace std::views;
    // ReSharper disable once CppLocalVariableMayBeConst
    auto view = _characters | values | filter(charHasDriver) | filter(charIsInRange) | filter(emitterIfRequested);

    for (auto const & c : view)
        c.player->connection().send(Packet(packet), src);
}

void World::broadcastToSubscribers(Actor const & emitter, Packet && packet, bool const includeEmitter,
                                   std::source_location const & src)
{
    for (auto const id : _targetSubscribers[emitter.id()])
    {
        // Ensure we find an active player
        if (auto const it = _characters.find(id); it != _characters.end() && it->second.player)
            it->second.player->connection().send(Packet(packet), src);
    }

    if (includeEmitter)
        send(emitter, Packet(packet));
}

// PRIVATE -------------------------------------------------------------------------------------------------------------

void World::delActor(Actor & a)
{
    if (a.type() == ActorType::Character)
    {
        auto & c = static_cast<Character &>(a);
        L2CPP_B_ASSERT(!c.player, "Illegal attempt to delete player driven character (objId: '{}')", a.id());
        moveCharacterBackToPreviews(c);
    }
    else
    {
        unsubscribeAllTargetListeners(a);
        broadcastAround(a, SC::GameObjectDeletePacket{a});
        _monsters.erase(a.id());
    }
}

std::vector<std::unique_ptr<System>>                             World::_systems;
std::unordered_map<std::wstring_view, std::vector<GameObjectId>> World::_characterPreviewsIndex;
std::unordered_map<GameObjectId, Character>                      World::_characterPreviews;
std::unordered_map<GameObjectId, Character>                      World::_characters;
std::unordered_map<GameObjectId, Monster>                        World::_monsters;
std::unordered_map<GameObjectId, Ref<Actor>>                     World::_scheduledForDeletion;
std::unordered_map<GameObjectId, std::list<GameObjectId>>        World::_targetSubscribers;
