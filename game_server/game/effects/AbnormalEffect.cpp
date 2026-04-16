/// @author    Chnossos
/// @date      Created on 2026-04-14

#include "AbnormalEffect.hpp"

// Project includes
#include "../World.hpp"
#include "../../network/packets/server/status/AbnormalEffectListPacket.hpp"
#include "../../utils/Chrono.hpp"
#include "../skill/Skill.hpp"
#include "network/Packet.hpp"

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
    , _finished(false)
{}

AbnormalEffect::~AbnormalEffect() = default;

auto AbnormalEffect::type() const -> AbnormalEffectType { return _type;     }
auto AbnormalEffect::skillUid() const -> SkillUid       { return _skillUid; }
auto AbnormalEffect::duration() const -> ClockDuration  { return _duration; }
auto AbnormalEffect::elapsed()  const -> ClockDuration  { return _elapsed;  }

auto AbnormalEffect::remainingDuration() const -> ClockDuration
{
    return _duration == ClockDuration::zero() ? ClockDuration::zero() : _duration - _elapsed;
}

auto AbnormalEffect::target() const -> Actor & { return _target; }

bool AbnormalEffect::update(ClockDuration const elapsed)
{
    if (_elapsed == ClockDuration::zero())
    {
        onStarted();

        if (_initialTriggerDuration == ClockDuration::zero())
            onTick();

        _elapsed += ClockDuration(1); // ensure we don't step inside this block again by making _elapsed non-zero
        return _finished;
    }

    /**/ if (Utils::Chrono::thresholdCrossed(_elapsed, elapsed, _duration))
    {
        setFinished(true);
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

    if (_finished)
        onFinished();
    else
        _elapsed += elapsed;

    return _finished;
}

void AbnormalEffect::setFinished(bool const finished) { _finished = finished; }

// ----------

DamageEffect::DamageEffect(
    Actor &                   target
    , SkillUid          const skillUid
    , DamageElementType const type
    , double            const damage
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

BuffEffect::BuffEffect(Actor & target, SkillUid const skillUid, ClockDuration const duration)
    : AbnormalEffect(AbnormalEffectType::Buff, target, skillUid, duration)
{}

void BuffEffect::onStarted()
{
    World::send(target(), Network::Packet::Server::AbnormalEffectListPacket{target()});
}
