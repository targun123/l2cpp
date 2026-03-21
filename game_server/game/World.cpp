/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "../network/packets/server/chat/ChatNpcSayPacket.hpp"
#include "actions/Attack.hpp"
#include "components/Gear.hpp"
#include "components/Position.hpp"

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
            auto & action = static_cast<AttackAction &>(c.currentAction()->get());
            if (action.startTime() >= action.lastUpdateTime()) // First tick
            {
                action.update(elapsed);

                // Attack once, use soulshots if a weapon is equipped
                auto flags = 0_u8;
                if (auto const weapon = c.gear().weapon(); weapon)
                    flags |= 0x10 | std::to_underlying(weapon->get().tmplate.grade);

                l2cpp::Network::Packet p(0x05);
                p
                    << c.id()
                    // Hit
                    << c.target()->get().id()
                    << 10 // dmg
                    << flags // flags: 0x10=use_ss 0x20=crit_sound 0x40=shield_sound 0x80=miss_sound
                    << c.position().x
                    << c.position().y
                    << c.position().z
                    << u16(0) // other hits (e.g. polearm)
                    << c.target()->get().position().x
                    << c.target()->get().position().y
                    << c.target()->get().position().z
                ;
                c.player->get().connection().send(p);
                return;
            }

            action.update(elapsed);
            if (action.lastUpdateTime() >= action.startTime() + action.hitDuration)
            {
                auto & player = c.player->get();

                // Enable attack stance on opponents
                player.connection().send(l2cpp::Network::Packet(0x2b) << c.target()->get().id());
                player.connection().send(l2cpp::Network::Packet(0x2b) << c.id());

                // Make the target go Ouch!
                Network::Packet::Server::ChatNpcSayPacket p(c.target()->get(), ChatType::General, L"Ouch!");
                player.connection().send(p);
                action.restart();
                // c.state = ActorState::Idle;
            }

            // if (action.lastUpdateTime() >= action.startTime() + action.hitDuration * 3)
            // {
            //     auto & player = c.player->get();
            //     // Stop stance after 2s
            //     player.connection().send(l2cpp::Network::Packet(0x2c) << c.id());
            //     player.connection().send(l2cpp::Network::Packet(0x2c) << c.target()->get().id());
            //
            //     c.state = ActorState::Idle;
            // }
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
