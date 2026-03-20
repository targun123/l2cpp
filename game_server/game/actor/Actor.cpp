/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../components/ActorIdentity.hpp"
#include "../components/Gear.hpp"
#include "../components/Position.hpp"
#include "../components/SkillDirectory.hpp"
#include "../components/Stats.hpp"

#include <l2cpp/details/Pimpl.hpp>

struct Actor::ActorImpl
{
    Team team = Team::None;
    bool isInCombatStance = false;

    OptionalRef<Actor const> target;
    std::deque<std::unique_ptr<Action>>            actionQueue;
};

template class Pimpl<Actor::ActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Actor::Actor()
{
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

auto Actor::target() const -> OptionalRef<Actor const> { return _impl->target; }

bool Actor::isInCombatStance() const { return _impl->isInCombatStance; }

auto Actor::actions() -> std::deque<std::unique_ptr<Action>> & { return _impl->actionQueue; }

void Actor::setName (std::wstring name)  { component<ActorIdentity>().name  = std::move(name);  }
void Actor::setTitle(std::wstring title) { component<ActorIdentity>().title = std::move(title); }

void Actor::setPosition(Position const & position)
{
    component<Position>() = position;
}

void Actor::setPosition(s32 const x, s32 const y, s32 const z)
{
    auto & pos = component<Position>();
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void Actor::setPosX(s32 const x) { component<Position>().x = x; }
void Actor::setPosY(s32 const y) { component<Position>().y = y; }
void Actor::setPosZ(s32 const z) { component<Position>().z = z; }

void Actor::setTeam(Team const team) { _impl->team = team; }

void Actor::setTarget(OptionalRef<Actor const> actor) { _impl->target = std::move(actor); }
