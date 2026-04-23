/// @author    Chnossos
/// @date      Created on 2026-04-16

#pragma once

// Project includes
#include "../constants/StatId.hpp"
#include "AbnormalEffectType.hpp"
#include "DamageElementType.hpp"

class Actor;
class SkillTemplate;

class AbnormalEffectFactory
{
public:
    explicit AbnormalEffectFactory(AbnormalEffectType const type, SkillTemplate const & skillTemplate)
        : _type(type)
        , _skillTemplate(skillTemplate)
    {}
    virtual ~AbnormalEffectFactory() = default;

public:
    auto type() const -> AbnormalEffectType { return _type; }

public:
    virtual void apply(Actor & source, Actor & target) = 0;

protected:
    AbnormalEffectType _type;
    SkillTemplate const & _skillTemplate;
};

class DamageEffectFactory : public AbnormalEffectFactory
{
public:
    DamageEffectFactory(SkillTemplate const & skillTemplate, DamageElementType const elementType)
        : AbnormalEffectFactory(AbnormalEffectType::Damage, skillTemplate)
        , _elementType(elementType)
    {}

public:
    void apply(Actor & source, Actor & target) override;

private:
    DamageElementType _elementType;
};

class BuffEffectFactory : public AbnormalEffectFactory
{
public:
    BuffEffectFactory(SkillTemplate const & skillTemplate, StatId const modifiedStat, double const value)
        : AbnormalEffectFactory(AbnormalEffectType::Buff, skillTemplate)
        , _modifiedStat(modifiedStat)
        , _value(value)
    {}

public:
    void apply(Actor & source, Actor & target) override;

private:
    StatId _modifiedStat;
    double _value;
};
