/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

// Project includes
#include "../Player.hpp"
#include "../network/Connection.hpp"
#include "../network/packets/server/chat/ChatNpcSayPacket.hpp"
#include "../network/packets/server/combat/AttackPacket.hpp"
#include "../network/packets/server/combat/AttackStanceTogglePacket.hpp"
#include "actions/Attack.hpp"
#include "components/Gear.hpp"

#include <l2cpp/network/Packet.hpp>

// C++ includes
#include <ranges>

namespace SM = Network::Packet::Server;

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
            auto & player = c.player->get();
            auto & target = c.target()->get();
            auto & action = static_cast<AttackAction &>(c.currentAction()->get());

            bool const isFirstTick = action.startTime() == action.lastUpdateTime();
            static bool impactDone = false;

            action.update(elapsed);
            if (isFirstTick) // Start a physical attack
            {
                // Use soulshots if a weapon is equipped
                std::optional<ItemGrade> soulShotGrade;
                if (auto const weapon = c.gear().weapon(); weapon)
                    soulShotGrade = weapon->get().tmplate.grade;

                player.connection().send(SM::AttackPacket(c, target, {target, 10, false, soulShotGrade}));
            }
            else if (action.lastUpdateTime() >= action.impactTimePoint && !impactDone)
            {
                // Enable attack stance on opponents
                player.connection().send(SM::AttackStanceTogglePacket(true, c));
                player.connection().send(SM::AttackStanceTogglePacket(true, target));

                // Make the target go Ouch!
                static bool toggle;
                player.connection().send(SM::ChatNpcSayPacket(target, ChatType::General, toggle ? L"Ouch!" : L"Waah!"));
                toggle = !toggle;

                impactDone = true;
            }
            else if (action.lastUpdateTime() >= action.startTime() + action.hitDuration)
            {
                // Continue attacking once the animation is complete
                action.restart();
                impactDone = false;
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
