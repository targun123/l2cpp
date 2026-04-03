/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "actor/Character.hpp"
#include "actor/Monster.hpp"
#include "ecs/System.hpp"

#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <unordered_map>
#include <vector>

namespace l2cpp::Network { class Packet; }

class World
{
    using SystemPtr = std::unique_ptr<System>;

private:
    World() noexcept = delete;

public:
    static auto character(GameObjectId) -> OptRef<Character>;
    static auto monster(GameObjectId)   -> OptRef<Monster>;

public:
    static void init();
    static void update(ClockDuration elapsed);

    template<typename T, typename... Args> requires std::is_base_of_v<System, T>
    static void registerSystem(Args &&... args) {
        _systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    static auto getCharacterPreviews(std::wstring_view playerAccount) -> std::vector<Ref<Character>>;
    static auto addCharacterPreview(std::wstring_view playerAccount) -> Character &;
    static auto loadCharacterFromPreview(Character &) -> Character &;
    static void moveCharacterBackToPreviews(Character &);

    static auto addCharacter(OptRef<Player> = std::nullopt) -> Character &;

    static auto addMonster() -> Monster &;
    static void delMonster(GameObjectId id);

    static auto inGameTime() -> std::chrono::minutes;

    static void subscribeToTarget(Actor const & target, Actor const & listener);
    static void unsubscribeFromTarget(Actor const & target, Actor const & listener);

    /// @warning This broadcasts given packet to ALL online players, regardless of distance!
    static void broadcast(l2cpp::Network::Packet &&);

    /// Broadcasts given packet to all online players around emitter, including the emitter if needed
    static void broadcastAround(Actor const & emitter, l2cpp::Network::Packet &&, bool includeEmitter = false);

    static void broadcastToSubscribers(Actor const & emitter, l2cpp::Network::Packet &&, bool includeEmitter = false);

private:
    static std::vector<SystemPtr> _systems;

    static std::unordered_map<std::wstring_view, std::vector<GameObjectId>> _characterPreviewsIndex;

    static std::unordered_map<GameObjectId, Character> _characterPreviews;
    static std::unordered_map<GameObjectId, Character> _characters;
    static std::unordered_map<GameObjectId, Monster>   _monsters;

    static std::unordered_map<GameObjectId, std::list<GameObjectId>> _targetSubscribers;
};
