/// @author    Chnossos
/// @date      Created on 2026-04-14

#include "AbnormalEffect.hpp"

// Project includes
#include "../../utils/Chrono.hpp"
#include "../actor/Character.hpp"
#include "../skill/Skill.hpp"

AbnormalEffect::AbnormalEffect(
    AbnormalEffectType const   type
    , Actor                  & source
    , Actor                  & target
    , SkillUid         const   skillUid
    , ClockDuration    const   effectDuration
    , ClockDuration    const   tickDuration
    , ClockDuration    const   initialTriggerDuration
)
    : _type(type)
    , _source(source)
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

auto AbnormalEffect::type()     const -> AbnormalEffectType { return _type;     }
auto AbnormalEffect::skillUid() const -> SkillUid           { return _skillUid; }
auto AbnormalEffect::duration() const -> ClockDuration      { return _duration; }
auto AbnormalEffect::elapsed()  const -> ClockDuration      { return _elapsed;  }

auto AbnormalEffect::remainingDuration() const -> ClockDuration
{
    return _duration > ClockDuration::zero() ? _duration - _elapsed : _duration;
}

auto AbnormalEffect::source() const -> Actor & { return _source; }
auto AbnormalEffect::target() const -> Actor & { return _target; }

bool AbnormalEffect::update(ClockDuration const elapsed)
{
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
        _finished = true;
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

    if (_finished)
        onFinished();
    else
        _elapsed += elapsed;

    return _finished;
}

void AbnormalEffect::cancel()
{
    _finished = true;
}
