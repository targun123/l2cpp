/// @author    Chnossos
/// @date      Created on 2026-04-14

#include "AbnormalEffect.hpp"

// Project includes
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../World.hpp"
#include "../../network/packets/server/status/CharacterStatusUpdateBroadcastPacket.hpp"
#include "../../network/packets/server/status/CharacterStatusUpdatePacket.hpp"
#include "../skill/Skill.hpp"

AbnormalEffect::AbnormalEffect(
    AbnormalEffectType const type
    , Actor &                target
    , SkillUid         const skillUid
    , ClockDuration    const effectDuration
    , ClockDuration    const tickDuration
    , ClockDuration    const initialTriggerDuration
)
    : _type(type)
    , _target(target)
    , _skillUid(skillUid)
    , _duration(effectDuration)
    , _elapsed(ClockDuration::zero())
    , _elapsedSinceLastTick(ClockDuration::zero())
    , _tickDuration(tickDuration)
    , _initialTriggerDuration(initialTriggerDuration)
{}

AbnormalEffect::~AbnormalEffect() = default;

auto AbnormalEffect::type()     const -> AbnormalEffectType { return _type;     }
auto AbnormalEffect::skillUid() const -> SkillUid           { return _skillUid; }
auto AbnormalEffect::duration() const -> ClockDuration      { return _duration; }
auto AbnormalEffect::elapsed()  const -> ClockDuration      { return _elapsed;  }

auto AbnormalEffect::remainingDuration() const -> ClockDuration
{
    return _duration == ClockDuration::zero() ? ClockDuration::zero() : _duration - _elapsed;
}

auto AbnormalEffect::target() const -> Actor & { return _target; }

bool AbnormalEffect::update(ClockDuration const elapsed)
{
    bool finished = false;

    if (_elapsed == ClockDuration::zero())
    {
        onStarted();

        if (_initialTriggerDuration == ClockDuration::zero())
            onTick();

        if (_duration > ClockDuration::zero())
            _elapsed  = ClockDuration(1); // ensure we don't step inside this block again by making _elapsed non-zero
    }

    /**/ if (Utils::Chrono::thresholdCrossed(_elapsed, elapsed, _duration))
    {
        finished = true;
    }
    else if (Utils::Chrono::thresholdCrossed(_elapsed, elapsed, _initialTriggerDuration))
    {
        onTick();
        _elapsedSinceLastTick = _elapsed + elapsed - _initialTriggerDuration;
    }
    else if (Utils::Chrono::thresholdCrossed(_elapsedSinceLastTick, elapsed, _tickDuration))
    {
        onTick();
        _elapsedSinceLastTick += elapsed - _tickDuration;
    }
    else
        _elapsedSinceLastTick += elapsed;

    if (finished)
        onFinished();
    else
        _elapsed += elapsed;

    return finished;
}

// ----------

DamageEffect::DamageEffect(
    Actor &                   target
    , SkillUid          const skillUid
    , DamageElementType const type
    , StatValue         const damage
    , ClockDuration     const effectDuration
    , ClockDuration     const tickDuration
    , ClockDuration     const initialTriggerDuration
)
    : AbnormalEffect(AbnormalEffectType::Damage, target, skillUid, effectDuration, tickDuration, initialTriggerDuration)
    , _elementType(type)
    , _damage(damage)
{}

void DamageEffect::onTick()
{
    target().takeDamage(_damage);
}

// ----------

BuffEffect::BuffEffect(
    Actor &               target
    , SkillUid      const skillUid
    , ClockDuration const duration
    , StatId        const modifiedStat
    , double        const value
)
    : AbnormalEffect(AbnormalEffectType::Buff, target, skillUid, duration)
    , _modifiedStat(modifiedStat)
    , _value(value)
{}

void BuffEffect::onStarted()
{
    modifyStat(_value);
}

void BuffEffect::onFinished()
{
    modifyStat(-_value);
}

void BuffEffect::modifyStat(double const newValue) const
{
    auto const & baseStats = *target().component<Stats>();
    auto       & stats     = *target().component<ComputedStats>();

    stats.runSpeed           += static_cast<StatValue>(newValue);
    stats.moveSpeedMultiplier = static_cast<double>(stats.runSpeed) / baseStats.runSpeed;

    namespace SC   = Network::Packet::Server;
    auto const & c = static_cast<Character &>(target());

    World::send           (target(), Network::Packet::Server::AbnormalEffectListPacket{target()});
    World::send           (target(), SC::CharacterStatusUpdatePacket{c});
    World::broadcastAround(target(), SC::CharacterStatusUpdateBroadcastPacket{c});
}
