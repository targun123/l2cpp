/// @author    Chnossos
/// @date      Created on 2026-03-15

#pragma once

// Project includes
#include "../GameObject.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Position.hpp"
#include "../constants/ActorState.hpp"
#include "../constants/Team.hpp"

#include <l2cpp/Pimpl.hpp>

// C++ includes
#include <deque>

class Gear;
class SkillDirectory;
struct ComputedStats;
struct Stats;

class Action
{
public:
    enum class Type
    {
        Move, Attack
    };

public:
    explicit Action(Type const type) noexcept: _type(type) {}
    virtual ~Action() noexcept = default;

public:
    Type type() const { return _type; }

private:
    Type _type;
};

struct AttackAction : public Action
{
    AttackAction()
        : Action(Type::Attack)
        , startTime(std::chrono::steady_clock::now())
        , lastUpdateTime(startTime)
    {}

    std::chrono::steady_clock::time_point startTime, lastUpdateTime;
};

struct MoveAction : public Action
{
    MoveAction() noexcept
        : Action(Type::Move)
        , startTime(std::chrono::steady_clock::now())
        , lastUpdateTime(startTime)
        , input()
    {}

    s32 originX = 0, originY = 0, originZ = 0;
    s32 targetX = 0, targetY = 0, targetZ = 0;

    float currentDistance = 0, totalDistance = 0;

    std::chrono::steady_clock::time_point startTime, lastUpdateTime;

    enum class Input { Keyboard, Mouse } input;
};

/// Base class for player characters, npcs, summons, pets…
class Actor : public GameObject, public Entity
{
public:
    ActorState state = ActorState::Idle;

public:
    Actor();
    Actor(Actor &&) noexcept;
    Actor & operator=(Actor &&) noexcept;
    ~Actor() override = 0;

public:
    auto name() const -> std::wstring_view;
    auto title() const -> std::wstring_view;
    auto position() const -> Position const &;
    auto team() const -> Team;

    auto baseStats() const -> Stats const &;
    auto stats() const -> ComputedStats const &;

    auto gear()       -> Gear       &;
    auto gear() const -> Gear const &;

    auto skills()       -> SkillDirectory &;
    auto skills() const -> SkillDirectory const &;

    auto target() const -> OptionalRef<Actor const>;
    bool isInCombatStance() const;

    auto actions() -> std::deque<std::unique_ptr<Action>> &;

public:
    void setName(std::wstring name);
    void setTitle(std::wstring title);
    void setPosition(Position const & position);
    void setPosition(s32 x, s32 y, s32 z);
    void setPosX(s32 x);
    void setPosY(s32 y);
    void setPosZ(s32 z);
    void setTeam(Team team);
    void setTarget(OptionalRef<Actor const>);

    template<typename A, typename... Args, typename = std::enable_if_t<std::is_base_of_v<Action, A>>>
    auto setNextAction(Args &&... args)
    {
        actions().clear();
        return static_cast<A &>(*actions().emplace_back(std::make_unique<A>(std::forward<Args>(args)...)));
    }

private:
    struct ActorImpl;
    Pimpl<ActorImpl> _impl;
};
