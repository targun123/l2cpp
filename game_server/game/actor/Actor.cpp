/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../../network/packets/server/status/ActorDiePacket.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../World.hpp"
#include "../components/ActorAutoRegen.hpp"
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

    OptRef<Actor> target;
    std::unique_ptr<Action> currentAction, nextAction;
};

template class Pimpl<Actor::ActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Actor::Actor(ActorType const type)
{
    _impl->type = type;

    addComponent<ActorAutoRegen>();
    addComponent<ActorIdentity>();
    addComponent<Gear>();
    addComponent<Position>();
    addComponent<SkillDirectory>();

    auto & baseStats = addComponent<Stats>();
    baseStats.maxCp         = 500;
    baseStats.maxHp         = 500;
    baseStats.maxMp         = 500;
    baseStats.STR           = 40;
    baseStats.DEX           = 30;
    baseStats.CON           = 43;
    baseStats.INT           = 21;
    baseStats.WIT           = 11;
    baseStats.MEN           = 25;
    baseStats.pAtk          = 10;
    baseStats.pDef          = 80;
    baseStats.mAtk          = 6;
    baseStats.mDef          = 40;
    baseStats.pAtkSpeed     = 300;
    baseStats.mAtkSpeed     = 333;
    baseStats.pAtkRange     = 20;
    baseStats.pAtkRandom    = 10;
    baseStats.accuracy      = 10;
    baseStats.evasion       = 10;
    baseStats.pCritRate     = 10;
    baseStats.mCritRate     = 10;
    baseStats.runSpeed      = 115;
    baseStats.walkSpeed     = 80;
    baseStats.swimRunSpeed  = 50;
    baseStats.swimWalkSpeed = 50;

    baseStats.hpRegen = 2.0;
    baseStats.mpRegen = 0.8;
    baseStats.cpRegen = 1.4;

    auto & stats = addComponent<ComputedStats>(baseStats);
    stats.curCp = stats.maxCp;
    stats.curHp = stats.maxHp;
    stats.curMp = stats.maxMp;
}

Actor::Actor(Actor &&) noexcept = default;
Actor & Actor::operator=(Actor &&) noexcept = default;
Actor::~Actor() = default;

bool Actor::operator==(Actor const & other) const { return id() == other.id(); }

auto Actor::type()     const -> ActorType         { return _impl->type;                       }
auto Actor::name()     const -> std::wstring_view { return component<ActorIdentity>()->name;  }
auto Actor::title()    const -> std::wstring_view { return component<ActorIdentity>()->title; }
auto Actor::position() const -> Position const &  { return component<Position>();             }
auto Actor::team()     const -> Team              { return _impl->team;                       }

auto Actor::baseStats()  const -> Stats         const & { return *component<Stats>();         }
auto Actor::stats()      const -> ComputedStats const & { return *component<ComputedStats>(); }

auto Actor::gear()       -> Gear       & { return *component<Gear>(); }
auto Actor::gear() const -> Gear const & { return *component<Gear>(); }

auto Actor::skills()       -> SkillDirectory       & { return *component<SkillDirectory>(); }
auto Actor::skills() const -> SkillDirectory const & { return *component<SkillDirectory>(); }

auto Actor::target() const -> OptRef<Actor> { return _impl->target; }

bool Actor::isAlive()          const { return stats().curHp > 0;       }
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

auto Actor::nextAction() -> OptRef<Action> { return _impl->nextAction ? OptRef(*_impl->nextAction) : std::nullopt; }

void Actor::setName (std::wstring name)  { component<ActorIdentity>()->name  = std::move(name);  }
void Actor::setTitle(std::wstring title) { component<ActorIdentity>()->title = std::move(title); }

void Actor::setPosition(Position const & position)             { *component<Position>() = position; }
void Actor::setPosition(s32 const x, s32 const y, s32 const z) { setPosition(Position{x, y, z});    }

void Actor::setPosX(s32 const x) { component<Position>()->x = x; }
void Actor::setPosY(s32 const y) { component<Position>()->y = y; }
void Actor::setPosZ(s32 const z) { component<Position>()->z = z; }

void Actor::setTeam(Team const team) { _impl->team = team; }

void Actor::setTarget(OptRef<Actor> actor) { _impl->target = std::move(actor); }

void Actor::cancelAction()
{
    if (auto const action = std::move(_impl->currentAction))
        action->cancel();

    _impl->nextAction.reset();
}

void Actor::takeDamage(double const amount)
{
    if (amount <= 0)
        return;

    bool dying = false;

    auto & stats = *component<ComputedStats>();
    if ((stats.curHp -= amount) <= 0)
    {
        stats.curHp = 0;
        dying = true;
    }

    Network::Packet::Server::StatsUpdatePacket p(*this);
    p.addStat(Stat::CurHp, static_cast<u32>(stats.curHp));
    World::broadcastToSubscribers(*this, std::move(p));

    if (dying)
    {
        World::broadcastAround(*this, Network::Packet::Server::ActorDiePacket(*this), true);

        if (this->type() != ActorType::Character || !static_cast<Character &>(*this).player)
            World::scheduleForDeletion(*this, 5s); // Corpse will disappear soon
    }
}

void Actor::doNext(std::unique_ptr<Action> action)
{
    if (!action)
        cancelAction();
    else if (!_impl->currentAction)
        _impl->currentAction = std::move(action);
    else if (_impl->currentAction->canBeInterruptedByAnotherAction())
    {
        _impl->currentAction = std::move(action);
        _impl->nextAction    = nullptr;
    }
    else
        _impl->nextAction = std::move(action);
}
