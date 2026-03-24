/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../components/ActorIdentity.hpp"
#include "../components/Gear.hpp"
#include "../components/Position.hpp"
#include "../components/SkillDirectory.hpp"
#include "../components/Stats.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

struct Actor::ActorImpl
{
    ActorType type;
    Team team = Team::None;
    bool isInCombatStance = false;

    OptRef<Actor const> target;
    std::unique_ptr<Action> currentAction, nextAction;
};

template class Pimpl<Actor::ActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Actor::Actor(ActorType const type)
{
    _impl->type = type;

    addComponent<ActorIdentity>();
    addComponent<Gear>();
    addComponent<Position>();
    addComponent<SkillDirectory>();

    auto & stats = addComponent<Stats>();
    stats.STR           = 40;
    stats.DEX           = 30;
    stats.CON           = 43;
    stats.INT           = 21;
    stats.WIT           = 11;
    stats.MEN           = 25;
    stats.pAtk          = 10;
    stats.pDef          = 80;
    stats.mAtk          = 6;
    stats.mDef          = 40;
    stats.pAtkSpeed     = 300;
    stats.mAtkSpeed     = 333;
    stats.pAtkRange     = 20;
    stats.pAtkRandom    = 10;
    stats.accuracy      = 10;
    stats.evasion       = 10;
    stats.pCritRate     = 10;
    stats.mCritRate     = 10;
    stats.runSpeed      = 115;
    stats.walkSpeed     = 80;
    stats.swimRunSpeed  = 50;
    stats.swimWalkSpeed = 50;
    addComponent<ComputedStats>(stats);
}

Actor::Actor(Actor &&) noexcept = default;
Actor & Actor::operator=(Actor &&) noexcept = default;
Actor::~Actor() = default;

auto Actor::type()     const -> ActorType         { return _impl->type;                      }
auto Actor::name()     const -> std::wstring_view { return component<ActorIdentity>().name;  }
auto Actor::title()    const -> std::wstring_view { return component<ActorIdentity>().title; }
auto Actor::position() const -> Position const &  { return component<Position>();            }
auto Actor::team()     const -> Team              { return _impl->team;                      }

auto Actor::baseStats()  const -> Stats         const & { return component<Stats>();         }
auto Actor::stats()      const -> ComputedStats const & { return component<ComputedStats>(); }

auto Actor::gear()       -> Gear       & { return component<Gear>(); }
auto Actor::gear() const -> Gear const & { return component<Gear>(); }

auto Actor::skills()       -> SkillDirectory       & { return component<SkillDirectory>(); }
auto Actor::skills() const -> SkillDirectory const & { return component<SkillDirectory>(); }

auto Actor::target() const -> OptRef<Actor const> { return _impl->target; }

bool Actor::isInCombatStance() const { return _impl->isInCombatStance; }

auto Actor::currentAction() -> OptRef<Action>
{
    OptRef<Action> action;

    if (_impl->currentAction && _impl->currentAction->isFinished())
        _impl->currentAction = std::move(_impl->nextAction);

    if (_impl->currentAction)
        action = *_impl->currentAction;

    return action;
}

auto Actor::nextAction() -> OptRef<Action>
{
    return _impl->nextAction ? OptRef(*_impl->nextAction) : std::nullopt;
}

void Actor::setName (std::wstring name)  { component<ActorIdentity>().name  = std::move(name);  }
void Actor::setTitle(std::wstring title) { component<ActorIdentity>().title = std::move(title); }

void Actor::setPosition(Position const & position)             { component<Position>() = position; }
void Actor::setPosition(s32 const x, s32 const y, s32 const z) { setPosition(Position{x, y, z});   }

void Actor::setPosX(s32 const x) { component<Position>().x = x; }
void Actor::setPosY(s32 const y) { component<Position>().y = y; }
void Actor::setPosZ(s32 const z) { component<Position>().z = z; }

void Actor::setTeam(Team const team) { _impl->team = team; }

void Actor::setTarget(OptRef<Actor const> actor) { _impl->target = std::move(actor); }

void Actor::doNext(std::unique_ptr<Action> action)
{
    if (!action)
    {
        _impl->currentAction.reset();
        _impl->nextAction.reset();
    }
    else if (!_impl->currentAction)                    // no action: set new action
        _impl->currentAction = std::move(action);
    else if (_impl->currentAction->canBeInterrupted()) // interruptible current action: replace with new action
    {
        doNext(nullptr);
        _impl->currentAction = std::move(action);
    }
    else                                               // uninterruptible current action: replace next / queue action
        _impl->nextAction = std::move(action);
}
