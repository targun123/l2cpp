/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "../constants/SkillTargetType.hpp"
#include "../constants/SkillType.hpp"
#include "../effects/AbnormalEffectFactory.hpp"
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
    auto effects()      const -> std::span<std::unique_ptr<AbnormalEffectFactory> const>;

public:
    void setCastDuration(MSec castDuration);
    void setType(SkillType type);

    template<class T> requires std::is_base_of_v<AbnormalEffectFactory, T>
    void addAbnormalEffectFactory(auto &&... args) {
        _effects.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
    }

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

    std::vector<std::unique_ptr<AbnormalEffectFactory>> _effects;
};
