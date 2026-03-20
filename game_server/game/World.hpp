/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "actor/Character.hpp"
#include "actor/Monster.hpp"

#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <unordered_map>

class World
{
private:
    World() noexcept = default;

public:
    static auto characters() -> std::unordered_map<GameObjectId, Character> const &;
    static auto monsters()   -> std::unordered_map<GameObjectId, Monster>   const &;

    static auto character(GameObjectId) -> OptionalRef<Character>;
    static auto monster(GameObjectId)   -> OptionalRef<Monster>;

public:
    static void update(ClockDuration elapsed);

    static auto addCharacter(OptionalRef<Player> = {}) -> Character &;
    static void delCharacter(GameObjectId id);

    static auto addMonster() -> Monster &;
    static void delMonster(GameObjectId id);

private:
    static std::unordered_map<GameObjectId, Character> _characters;
    static std::unordered_map<GameObjectId, Monster>   _monsters;
};
