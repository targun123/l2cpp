/// @author    Chnossos
/// @date      Created on 2026-04-16

#pragma once

// Project includes
#include "../constants/StatId.hpp"
#include "AbnormalEffectType.hpp"
#include "DamageElementType.hpp"

#include <l2cpp/Typedefs.hpp>

class Actor;
class SkillTemplate;

class AbnormalEffectFactory
{
public:
    AbnormalEffectFactory(
        AbnormalEffectType         type
      , SkillTemplate      const & skillTemplate
      , ClockDuration              totalDuration          = ClockDuration::zero()
      , ClockDuration              tickDuration           = ClockDuration::zero()
      , ClockDuration              initialTriggerDuration = ClockDuration::zero()
    );
    virtual ~AbnormalEffectFactory() = default;

public:
    auto type() const -> AbnormalEffectType { return _type; }

public:
    virtual void apply(Actor & source, Actor & target) = 0;

protected:
    SkillTemplate const & _skillTemplate;
    ClockDuration         _totalDuration;
    ClockDuration         _tickDuration;
    ClockDuration         _initialTriggerDuration;

private:
    AbnormalEffectType _type;
};

class DamageEffectFactory : public AbnormalEffectFactory
{
public:
    DamageEffectFactory(
        SkillTemplate     const & skillTemplate
      , DamageElementType const   elementType
      , u32               const   power
      , ClockDuration     const   totalDuration          = ClockDuration::zero()
      , ClockDuration     const   tickDuration           = ClockDuration::zero()
      , ClockDuration     const   initialTriggerDuration = ClockDuration::zero()
    )
        : AbnormalEffectFactory(AbnormalEffectType::Damage, skillTemplate,
                                totalDuration, tickDuration, initialTriggerDuration)
        , _elementType(elementType)
        , _power(power)
    {}

public:
    void apply(Actor & source, Actor & target) override;

private:
    DamageElementType _elementType;
    u32               _power;
};

class HealEffectFactory : public AbnormalEffectFactory
{
public:
    HealEffectFactory(
        SkillTemplate     const & skillTemplate
      , u32               const   power
      , ClockDuration     const   totalDuration          = ClockDuration::zero()
      , ClockDuration     const   tickDuration           = ClockDuration::zero()
      , ClockDuration     const   initialTriggerDuration = ClockDuration::zero()
    )
        : AbnormalEffectFactory(AbnormalEffectType::Damage, skillTemplate,
                                totalDuration, tickDuration, initialTriggerDuration)
        , _power(power)
    {}

public:
    void apply(Actor & source, Actor & target) override;

private:
    u32 _power;
};

class BuffEffectFactory : public AbnormalEffectFactory
{
public:
    BuffEffectFactory(SkillTemplate const & skillTemplate, ClockDuration const duration,
                      StatId const modifiedStat, double const value)
        : AbnormalEffectFactory(AbnormalEffectType::Buff, skillTemplate, duration)
        , _modifiedStat(modifiedStat)
        , _value(value)
    {}

public:
    void apply(Actor & source, Actor & target) override;

private:
    StatId _modifiedStat;
    double _value;
};

struct ToggleBuffEffectFactory : public BuffEffectFactory
{
    ToggleBuffEffectFactory(SkillTemplate const & skillTemplate, StatId const modifiedStat, double const value)
        : BuffEffectFactory(skillTemplate, -1s, modifiedStat, value)
    {}
};
