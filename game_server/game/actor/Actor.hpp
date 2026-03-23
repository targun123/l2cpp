/// @author    Chnossos
/// @date      Created on 2026-03-15

#pragma once

// Project includes
#include "../GameObject.hpp"
#include "../actions/Action.hpp"
#include "../constants/ActorState.hpp"
#include "../constants/ActorType.hpp"
#include "../constants/Team.hpp"
#include "../ecs/Entity.hpp"

#include <l2cpp/Pimpl.hpp>

class Gear;
class SkillDirectory;
struct ComputedStats;
struct Position;
struct Stats;

/// Base class for player characters, npcs, summons, pets…
class Actor : public GameObject, public Entity
{
public:
    ActorState state = ActorState::Idle;

public:
    explicit Actor(ActorType type);
    Actor(Actor &&) noexcept;
    Actor & operator=(Actor &&) noexcept;
    ~Actor() override = 0;

public:
    auto type() const -> ActorType;
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

    auto currentAction() const -> OptionalRef<Action>;
    auto nextAction() const -> OptionalRef<Action>;

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

    auto setNextAction(std::unique_ptr<Action>) -> Action &;
    template<typename A, typename... Args> requires std::is_base_of_v<Action, A>
    auto setNextAction(Args &&... args) -> A &
    {
        return static_cast<A &>(setNextAction(std::make_unique<A>(std::forward<Args>(args)...)));
    }

private:
    struct ActorImpl;
    Pimpl<ActorImpl> _impl;
};
