/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "World.hpp"

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

auto World::addCharacter() -> Character &
{
    Character character;
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
