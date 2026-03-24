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

    static auto character(GameObjectId) -> OptRef<Character>;
    static auto monster(GameObjectId)   -> OptRef<Monster>;

public:
    static void update(ClockDuration elapsed);

    static auto addCharacter(OptRef<Player> = {}) -> Character &;
    static void delCharacter(GameObjectId id);

    static auto addMonster() -> Monster &;
    static void delMonster(GameObjectId id);

    static auto inGameTime() -> std::chrono::minutes;

private:
    static std::unordered_map<GameObjectId, Character> _characters;
    static std::unordered_map<GameObjectId, Monster>   _monsters;
};
