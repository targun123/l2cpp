/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "../network/packets/server/target/TargetClearPacket.hpp"
#include "../network/packets/server/world/GameObjectDeletePacket.hpp"
#include "../utils/Conversion.hpp"
#include "../utils/Maths.hpp"
#include "actor/Character.hpp"
#include "actor/Monster.hpp"
#include "actor/Npc.hpp"
#include "actor/NpcDirectory.hpp"
#include "components/ActorStatus.hpp"
#include "components/DeletionTimer.hpp"
#include "components/Loot.hpp"
#include "components/NpcAppearance.hpp"
#include "components/PlayerAppearance.hpp"
#include "components/Position.hpp"
#include "constants/Profession.hpp"
#include "constants/Race.hpp"
#include "constants/Sex.hpp"
#include "ecs/System.hpp"
#include "systems/ActorAttackStanceTimerSystem.hpp"
#include "systems/ActorAutoRegenSystem.hpp"
#include "systems/ActorDeletionTimerSystem.hpp"
#include "systems/ActorSkillEffectSystem.hpp"

#include <l2cpp/CompileTimeConfig.hpp>

// Third-party
#include <spdlog/spdlog.h>

// C++ includes
#include <ranges>

namespace SC = Network::Packet::Server; // Server -> Client

using l2cpp::Network::Packet;

static void addGremlin()
{
    static u32 count = 1;

    if (auto const gremlin = World::addMonster(1))
    {
        gremlin->setPosX(gremlin->position().x + (count++ % 2 ? 35 : -35));
        gremlin->setPosY(gremlin->position().y + (count++ % 2 ? 35 : -35));

        auto & loot = gremlin->addComponent<Loot>();
        loot.xp = 29;
        loot.sp =  2;
    }
}

static void addDummy()
{
    static u32 count = 1;

    auto & d = World::addCharacter();
    d.setPosY(d.position().y + (count++ % 2 ? 35 : -35));
    d.setName(std::format(L"dummy{}", d.id()));
    d.appearance().setRace(Race::Elf);
    d.appearance().sex = Sex::Female;
    d.appearance().collisionHeight = 23;
    d.appearance().collisionRadius = 7.5;
    d.setProfession(Profession::ElvenMystic);
}

void World::init()
{
    registerSystem<ActorAttackStanceTimerSystem>();
    registerSystem<ActorAutoRegenSystem>();
    registerSystem<ActorDeletionTimerSystem>();
    registerSystem<ActorSkillEffectSystem>();

    auto & c1 = addCharacterPreview(L"Admin");
    c1.setName(L"test" + std::to_wstring(c1.id()));
    c1.setTitle(L"Admin");

    auto & c2 = addCharacterPreview(L"Admin2");
    c2.setName(L"test" + std::to_wstring(c2.id()));
    c2.setTitle(L"Admin2");

    addGremlin();
    addGremlin();

    addDummy();
    addDummy();
}

auto World::actor(GameObjectId const id) -> OptRef<Actor>
{
    auto const it = _actors.find(id);
    return it != _actors.end() ? OptRef(*it->second) : std::nullopt;
}

auto World::character(GameObjectId const id) -> OptRef<Character>
{
    auto const actor = World::actor(id);
    return actor && actor->type() == ActorType::Character ? OptRef(static_cast<Character &>(*actor)) : std::nullopt;
}

auto World::monster(GameObjectId const id) -> OptRef<Monster>
{
    auto const actor = World::actor(id);
    return actor && actor->type() == ActorType::Monster ? OptRef(static_cast<Monster &>(*actor)) : std::nullopt;
}

auto World::npc(GameObjectId const id) -> OptRef<Npc>
{
    auto const actor = World::actor(id);
    return actor && actor->type() == ActorType::Npc ? OptRef(static_cast<Npc &>(*actor)) : std::nullopt;
}

void World::update(ClockDuration const elapsed)
{
    using namespace std::chrono;

    for (auto const & a : _actors | std::views::values)
    {
        try
        {
            if (auto const action = a->currentAction())
                action->update(elapsed);
        }
        catch (l2cpp::Exception const & e)
        {
            SPDLOG_ERROR("Failed to update action for character '{}':\n{}", a->id(), l2cpp::formatExceptionStack(e));
        }
        catch (std::exception const & e)
        {
            SPDLOG_ERROR("Failed to update action for character '{}':\n{}", a->id(), l2cpp::formatExceptionStack(e));
        }
    }

    for (auto const & system : _systems)
        for (auto const & a : _actors | std::views::values) system->update(elapsed, *a);

    // Death is handled outside the system loops because internals are modified upon death
    for (auto const & a : _actors | std::views::values) if (a->dying()) a->die();

    for (Actor & a : _scheduledForDeletion | std::views::values)
        delActor(a);

    _scheduledForDeletion.clear();
}

auto World::getCharacterPreviews(std::wstring_view const playerAccount) -> std::vector<Ref<Character>>
{
    std::vector<Ref<Character>> result;

    auto const & index = _characterPreviewsIndex[playerAccount];
    for (result.reserve(index.size()); auto const id : index)
        result.emplace_back(*_characterPreviews.at(id));

    return result;
}

auto World::addCharacterPreview(std::wstring_view const playerAccount) -> Character &
{
    L2CPP_B_ASSERT(!playerAccount.empty(), "Player account name unknown, cannot create character preview");

    Character  c;
    auto const id = c.id();
    _characterPreviewsIndex[playerAccount].emplace_back(id);
    return *_characterPreviews.try_emplace(id, std::make_unique<Character>(std::move(c))).first->second;
}

auto World::loadCharacterFromPreview(Character & c) -> Character &
{
    auto const id    = c.id();
    auto const & ptr = _actors.try_emplace(id, std::make_unique<Character>(std::move(c))).first->second;
    _characterPreviews.erase(id);
    return static_cast<Character &>(*ptr);
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
    _characterPreviews.try_emplace(id, std::make_unique<Character>(std::move(c)));
    _actors.erase(id);
}

auto World::addCharacter(OptRef<Player> p) -> Character & { return addActor<Character>(std::move(p)); }

auto World::addMonster(u32 const id) -> OptRef<Monster>
{
    auto const result = addNpc(id);
    return result && result->type() == ActorType::Monster ? OptRef(static_cast<Monster &>(*result)) : std::nullopt;
}

auto World::addNpc(u32 id) -> OptRef<Npc>
{
    OptRef<Npc> npc;

    if (auto const info = NpcDirectory::find(id))
    {
        npc = info->type == ActorType::Npc ? addActor<Npc>(id) : addActor<Monster>(id);
        npc->setName(Utils::toWideString(info->name));

        if (info->title.empty())
            npc->setTitle(std::format(L"Lv. {}", npc->status().level()));
        else
            npc->setTitle(Utils::toWideString(info->title));

        npc->appearance().collisionHeight = 15;
        npc->appearance().collisionRadius = 10;
    }

    return npc;
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

void World::unscheduleForDeletion(Actor & a)
{
    _scheduledForDeletion.erase(a.id());
    a.delComponent<DeletionTimer>();
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

auto World::subscribeToTarget(GameObjectId const targetId, Actor const & listener) -> OptRef<Actor>
{
    OptRef<Actor> target;

    if (listener.type() == ActorType::Character)
    {
        if (target = actor(targetId); target)
            subscribeToTarget(target, listener);
    }

    return target;
}

void World::subscribeToTarget(Actor const & target, Actor const & listener)
{
    if (listener.type() != ActorType::Character || listener == target) // do not subscribe to yourself
        return;

    if (auto const currentTarget = listener.target())
        unsubscribeFromTarget(currentTarget, listener);

    _targetSubscribers[target.id()].emplace_back(listener.id());
}

void World::unsubscribeFromTarget(Actor const & target, Actor const & listener)
{
    _targetSubscribers[target.id()].remove(listener.id());
}

void World::unsubscribeAllTargetListeners(Actor const & target)
{
    using namespace std::views;
    for (auto const c : _targetSubscribers[target.id()] | transform([] (auto const id) { return character(id); }))
    {
        if (c)
        {
            c->setTarget(std::nullopt);
            send(c, SC::TargetClearPacket{c});
        }
    }

    _targetSubscribers[target.id()].clear();
}

void World::forEachActorAround(Actor const & source, std::function<void(Actor &)> const & f)
{
    if (f)
    {
        auto const distancePred = [&] (auto const & a) { return isInBroadcastRange(source, *a); };
        auto const skipEmitter  = [&] (auto const & a) { return *a != source;                   };

        using namespace std::views;
        for (auto & a : _actors | values | filter(distancePred) | filter(skipEmitter))
            f(*a);
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
    for (auto const & a : _actors | std::views::values)
        send(*a, Packet(packet), src);
}

void World::broadcastAround(Actor const & emitter, Packet && packet, bool const includeEmitter,
                            std::source_location const & src)
{
    auto const charIsInRange      = [&] (auto const & a) { return isInBroadcastRange(emitter, *a); };
    auto const emitterIfRequested = [&] (auto const & a) { return *a != emitter || includeEmitter; };

    using namespace std::views;
    for (auto const & a : _actors | values | filter(charIsInRange) | filter(emitterIfRequested))
        send(*a, Packet(packet), src);
}

void World::broadcastToSubscribers(Actor const & emitter, Packet && packet, bool const includeEmitter,
                                   std::source_location const & src)
{
    auto & subs = _targetSubscribers[emitter.id()];

    for (auto it = subs.begin(); it != subs.end(); )
    {
        if (auto const actorIt = _actors.find(*it); actorIt != _actors.end())
        {
            send(*actorIt->second, Packet(packet), src);
            ++it;
        }
        else
            it = subs.erase(it);
    }

    if (includeEmitter)
        send(emitter, Packet(packet));
}

auto World::addActor(std::unique_ptr<Actor> actor) -> Actor &
{
    auto const id = actor->id();
    return *_actors.try_emplace(id, std::move(actor)).first->second;
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
        _actors.erase(a.id());
    }
}

std::vector<std::unique_ptr<System>>                             World::_systems;
std::unordered_map<std::wstring_view, std::vector<GameObjectId>> World::_characterPreviewsIndex;
std::unordered_map<GameObjectId, std::unique_ptr<Character>>     World::_characterPreviews;
std::unordered_map<GameObjectId, std::unique_ptr<Actor>>         World::_actors;
std::unordered_map<GameObjectId, Ref<Actor>>                     World::_scheduledForDeletion;
std::unordered_map<GameObjectId, std::list<GameObjectId>>        World::_targetSubscribers;
