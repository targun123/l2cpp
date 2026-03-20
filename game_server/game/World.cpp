/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "components/NpcAppearance.hpp"
#include "components/Stats.hpp"

#include <l2cpp/network/Packet.hpp>

// C++ includes
#include <ranges>

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

auto World::character(GameObjectId const id) -> OptionalRef<Character>
{
    OptionalRef<Character> result;

    if (auto const it = _characters.find(id); it != _characters.end())
        result = it->second;

    return result;
}

auto World::monster(GameObjectId const id) -> OptionalRef<Monster>
{
    OptionalRef<Monster> result;

    if (auto const it = _monsters.find(id); it != _monsters.end())
        result = it->second;

    return result;
}

void World::update(ClockDuration const elapsed)
{
    using namespace std::chrono;

    for (auto & c : _characters | std::views::values)
    {
        if (c.state == ActorState::Attacking)
        {
            auto & action = static_cast<AttackAction &>(*c.actions().front());

            auto const nextHit = action.startTime + MSec{static_cast<u32>(c.stats().pAtkSpeed / 500. * 1000 * 1.1)};

            action.lastUpdateTime += elapsed;

            if (action.lastUpdateTime >= nextHit)
            {
                auto & player = c.player->get();

                // Enable attack stance on opponents
                player.connection().send(l2cpp::Network::Packet(0x2b) << c.target()->get().id());
                player.connection().send(l2cpp::Network::Packet(0x2b) << c.id());

                // Make the target go Ouch!
                l2cpp::Network::Packet p(0x02);
                p << c.target()->get().id() << 0 << static_cast<Monster const &>(c.target()->get()).appearance().id()
                  << L"Ouch!";
                c.player->get().connection().send(p);
            }

            if (action.lastUpdateTime >= (nextHit + 2s))
            {
                auto & player = c.player->get();
                // Stop stance after 2s
                player.connection().send(l2cpp::Network::Packet(0x2c) << c.id());
                player.connection().send(l2cpp::Network::Packet(0x2c) << c.target()->get().id());

                c.state = ActorState::Idle;
            }
        }
    }
}

auto World::addCharacter(OptionalRef<Player> p) -> Character &
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
