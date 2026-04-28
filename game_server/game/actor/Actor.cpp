/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../../Player.hpp"
#include "../../network/Connection.hpp"
#include "../../network/packets/server/action/SocialActionPerformPacket.hpp"
#include "../../network/packets/server/chat/ChatSystemSayPacket.hpp"
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
#include "../../network/packets/server/status/ActorDiePacket.hpp"
#include "../../network/packets/server/status/ActorRevivePacket.hpp"
#include "../../network/packets/server/status/StatsUpdatePacket.hpp"
#include "../World.hpp"
#include "../components/ActorAutoRegen.hpp"
#include "../components/ActorIdentity.hpp"
#include "../components/CharacterStatus.hpp"
#include "../components/Gear.hpp"
#include "../components/Loot.hpp"
#include "../components/Position.hpp"
#include "../components/SkillDirectory.hpp"
#include "../components/Stats.hpp"
#include "../gameplay/ExperienceTable.hpp"
#include "Character.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

namespace SC = Network::Packet::Server; // Server -> Client

struct Actor::ActorImpl
{
    ActorType type;
    Team team = Team::None;

    bool dying = false;
    bool isInCombatStance = false;

    OptRef<Actor> target, lastHitEmitter;
    std::unique_ptr<Action> currentAction, nextAction;

    std::list<std::unique_ptr<AbnormalEffect>> _abnormalEffects;
};

template class Pimpl<Actor::ActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Actor::Actor(ActorType const type)
{
    _impl->type = type;

    addComponent<ActorIdentity>();
    addComponent<Gear>();
    addComponent<Position>(-83968, 244634, -3500); // Talking Island GK
    // addComponent<Position>(-71338, 258271, -3104); // Human Fighter creation spawn point

    auto & skills = addComponent<SkillDirectory>();
    skills.learn(18,   1); // Hate Aura
    skills.learn(78,   1); // War Cry
    skills.learn(129,  1); // Poison
    skills.learn(1016, 1); // Resurrection
    skills.learn(1027, 1); // Group Heal
    skills.learn(1177, 1); // Wind Strike
    skills.learn(1204, 1); // Wind Walk
    skills.learn(1216, 1); // Self Heal
    skills.learn(1217, 1); // Greater Heal
    skills.learn(1229, 1); // Chant of Life
    skills.learn(1254, 1); // Mass Resurrection
    skills.learn(1256, 1); // Heart of Paagrio
    skills.learn(1295, 1); // Aqua Splash

    auto & stats = addComponent<Stats>();
    stats[StatId::BaseStr]           = 40;
    stats[StatId::BaseDex]           = 30;
    stats[StatId::BaseCon]           = 43;
    stats[StatId::BaseInt]           = 21;
    stats[StatId::BaseWit]           = 11;
    stats[StatId::BaseMen]           = 25;
    stats[StatId::BasePAtk]          = 4;
    stats[StatId::BasePDef]          = 80;
    stats[StatId::BaseMAtk]          = 6;
    stats[StatId::BaseMDef]          = 41;
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

bool Actor::dying()   const { return _impl->dying;                }
bool Actor::isAlive() const { return stats()[StatId::CurHp] >= 1; }
bool Actor::isDead()  const { return stats()[StatId::CurCp] < 1;  }

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

void Actor::takeDamage(OptRef<Actor> emitter, double const amount)
{
    if (!isAlive() || amount == 0)
        return;

    _impl->lastHitEmitter = std::move(emitter);

    auto & stats = *component<Stats>();
    auto & hp    = stats[StatId::CurHp] -= amount;

    if (hp < 1)
    {
        hp = 0;
        _impl->dying = true;
    }
    else if (hp > stats[StatId::MaxHp])
        hp = stats[StatId::MaxHp];

    SC::StatsUpdatePacket p(*this);
    p.addStat(Stat::CurHp, static_cast<u32>(stats[StatId::CurHp]));
    World::broadcastToSubscribers(*this, std::move(p), true);
}

void Actor::die()
{
    if (!isAlive() && !_impl->dying)
        return;

    _impl->dying = false;

    _impl->_abnormalEffects.clear();
    World::send(*this, SC::AbnormalEffectListPacket{*this});

    delComponent<ActorAutoRegen>();
    delComponent<AttackStanceTimer>();

    if (auto const emitter = _impl->lastHitEmitter; emitter && emitter->type() == ActorType::Character)
    {
        auto & c = static_cast<Character &>(*emitter);
        if (auto const loot = component<Loot>())
        {
            SC::StatsUpdatePacket p(c);

            bool const nonZeroXp = loot->xp > 0;
            if (nonZeroXp)
            {
                c.status().xp += loot->xp;
                p.addStat(Stat::Xp, c.status().xp);
            }

            bool leveledUp = false;
            auto const currentLevel = c.status().level();
            if (auto const level = ExperienceTable::level(c.status().xp); level > currentLevel)
            {
                leveledUp = true;
                c.status().setLevel(level);
                p.addStat(Stat::Level, level);
            }

            bool const nonZeroSp = loot->sp > 0;
            if (nonZeroSp)
            {
                c.status().sp += loot->sp;
                p.addStat(Stat::Sp, c.status().sp);
            }
            c.player->connection().send(p);

            std::optional<SC::ChatSystemSayPacket> msg;
            /**/ if (nonZeroXp && nonZeroSp)
            {
                msg.emplace(SystemMessageId::EarnedXpAndSp);
                *msg << SysMsgArg::Number{loot->xp} << SysMsgArg::Number{loot->sp};
            }
            else if (nonZeroXp)
            {
                msg.emplace(SystemMessageId::EarnedXp);
                *msg << SysMsgArg::Number{loot->xp};
            }
            else
            {
                msg.emplace(SystemMessageId::EarnedSp);
                *msg << SysMsgArg::Number{loot->sp};
            }
            c.player->connection().send(*msg);

            if (leveledUp)
            {
                World::broadcastAround(c, SC::SocialActionPerformPacket{c, SocialAction::LevelUpAnimation}, true);
                c.player->connection().send(SC::ChatSystemSayPacket{SystemMessageId::YourLevelHasIncreased});
            }
        }
    }

    World::broadcastAround(*this, SC::ActorDiePacket(*this), true);

    if (this->type() != ActorType::Character || !static_cast<Character &>(*this).player)
        World::scheduleForDeletion(*this, 5s); // Corpse will disappear soon
}

void Actor::revive()
{
    if (isAlive())
        return;
    addComponent<ActorAutoRegen>();

    World::unscheduleForDeletion(*this);
    World::broadcastAround(*this, SC::ActorRevivePacket{*this}, true);

    auto & stats = *component<Stats>();
    stats[StatId::CurHp] = stats[StatId::MaxHp] * 0.65;

    SC::StatsUpdatePacket p(*this);
    p.addStat(Stat::CurHp, stats[StatId::CurHp]);
    World::broadcastToSubscribers(*this, std::move(p), true);
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
