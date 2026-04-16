/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "../constants/SkillTargetType.hpp"
#include "../constants/SkillType.hpp"
#include "../effects/AbnormalEffect.hpp"
#include "SkillUid.hpp"

// C++ includes
#include <span>

class SkillTemplate
{
public:
    SkillTemplate(SkillId id, std::string name, SkillLevel lvl);
    SkillTemplate(SkillId id, std::string name, SkillLevel lvl, SkillType type, SkillTargetType targetType,
                  MSec castDuration, MSec cdDuration);

public:
    auto uid()          const -> SkillUid;
    auto id()           const -> SkillId;
    auto name()         const -> std::string_view;
    auto fullName()     const -> std::string_view;
    auto level()        const -> SkillLevel;
    auto type()         const -> SkillType;
    auto castDuration() const -> MSec;
    auto effects()      const -> std::span<AbnormalEffectType const>;

public:
    void setCastDuration(MSec castDuration);
    void setType(SkillType type);
    void setEffectTypes(std::vector<AbnormalEffectType> effects);

private:
    SkillId         _id;
    SkillLevel      _level;
    SkillType       _type;
    SkillTargetType _targetType;
    std::string     _name;
    std::string     _fullName;
    std::string     _icon;
    MSec            _castDuration;
    MSec            _cooldownDuration;

    std::vector<AbnormalEffectType> _effects;
};
