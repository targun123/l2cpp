/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
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

    bool dying = false;
    bool isInCombatStance = false;

    OptRef<Actor> target;
    std::unique_ptr<Action> currentAction, nextAction;

    std::list<std::unique_ptr<AbnormalEffect>> _abnormalEffects;
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

    auto & skills = addComponent<SkillDirectory>();
    skills.learn(129,  1); // Poison
    skills.learn(1204, 1); // Wind Walk
    skills.learn(1177, 1); // Wind Strike

    auto & stats = addComponent<Stats>();
    stats[StatId::BaseStr]           = 40;
    stats[StatId::BaseDex]           = 30;
    stats[StatId::BaseCon]           = 43;
    stats[StatId::BaseInt]           = 21;
    stats[StatId::BaseWit]           = 11;
    stats[StatId::BaseMen]           = 25;
    stats[StatId::BasePAtk]          = 10;
    stats[StatId::BasePDef]          = 80;
    stats[StatId::BaseMAtk]          = 6;
    stats[StatId::BaseMDef]          = 40;
    stats[StatId::BasePAtkSpeed]     = 300;
    stats[StatId::BaseMAtkSpeed]     = 333;
    stats[StatId::BasePAtkRange]     = 20;
    stats[StatId::BasePAtkRandom]    = 10;
    stats[StatId::BaseAccuracy]      = 10;
    stats[StatId::BaseEvasion]       = 10;
    stats[StatId::BasePCritRate]     = 10;
    stats[StatId::BaseMCritRate]     = 10;
    stats[StatId::BaseRunSpeed]      = 115;
    stats[StatId::BaseWalkSpeed]     = 80;
    stats[StatId::BaseSwimRunSpeed]  = 50;
    stats[StatId::BaseSwimWalkSpeed] = 50;

    stats[StatId::BaseHpRegen] = 2.0;
    stats[StatId::BaseMpRegen] = 0.8;
    stats[StatId::BaseCpRegen] = 1.4;

    stats[StatId::BaseMaxCp] = 500;
    stats[StatId::BaseMaxHp] = 500;
    stats[StatId::BaseMaxMp] = 500;

    stats.compute(*this);

    stats[StatId::CurCp] = stats[StatId::MaxCp];
    stats[StatId::CurHp] = stats[StatId::MaxHp];
    stats[StatId::CurMp] = stats[StatId::MaxMp];
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

auto Actor::stats()       -> Stats       & { return *component<Stats>(); }
auto Actor::stats() const -> Stats const & { return *component<Stats>(); }

auto Actor::gear()       -> Gear       & { return *component<Gear>(); }
auto Actor::gear() const -> Gear const & { return *component<Gear>(); }

auto Actor::skills()       -> SkillDirectory       & { return *component<SkillDirectory>(); }
auto Actor::skills() const -> SkillDirectory const & { return *component<SkillDirectory>(); }

auto Actor::target() const -> OptRef<Actor> { return _impl->target; }

bool Actor::dying()            const { return _impl->dying;               }
bool Actor::isAlive()          const { return stats()[StatId::CurHp] > 0; }
bool Actor::isInCombatStance() const { return _impl->isInCombatStance;    }

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

auto Actor::abnormalEffects() -> std::list<std::unique_ptr<AbnormalEffect>> & { return _impl->_abnormalEffects; }
auto Actor::abnormalEffects() const -> std::list<std::unique_ptr<AbnormalEffect>> const & {
    return _impl->_abnormalEffects;
}

// SETTERS -------------------------------------------------------------------------------------------------------------

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

    auto & stats = *component<Stats>();
    if (stats[StatId::CurHp] == 0)
        return;

    if ((stats[StatId::CurHp] -= amount) <= 0)
    {
        stats[StatId::CurHp] = 0;
        _impl->dying = true;
    }

    Network::Packet::Server::StatsUpdatePacket p(*this);
    p.addStat(Stat::CurHp, static_cast<u32>(stats[StatId::CurHp]));
    World::broadcastToSubscribers(*this, std::move(p), true);
}

void Actor::die()
{
    _impl->dying = false;

    _impl->_abnormalEffects.clear();
    World::send(*this, Network::Packet::Server::AbnormalEffectListPacket{*this});

    delComponent<ActorAutoRegen>();

    World::broadcastAround(*this, Network::Packet::Server::ActorDiePacket(*this), true);

    if (this->type() != ActorType::Character || !static_cast<Character &>(*this).player)
        World::scheduleForDeletion(*this, 5s); // Corpse will disappear soon
}

void Actor::resurrect()
{
    addComponent<ActorAutoRegen>();
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

void Actor::addAbnormalEffect(std::unique_ptr<AbnormalEffect> e) { _impl->_abnormalEffects.emplace_back(std::move(e)); }
