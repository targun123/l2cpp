/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "../constants/SkillTargetNature.hpp"
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

public:
    auto uid()              const -> SkillUid;
    auto id()               const -> SkillId;
    auto name()             const -> std::string_view;
    auto fullName()         const -> std::string_view;
    auto level()            const -> SkillLevel;
    auto type()             const -> SkillType;
    auto targetType()       const -> SkillTargetType;
    auto targetNature()     const -> SkillTargetNature;
    auto isMagic()          const -> bool;
    auto needsTarget()      const -> bool;
    auto castDuration()     const -> ClockDuration;
    auto cooldownDuration() const -> ClockDuration;
    auto effects()          const -> std::span<std::unique_ptr<AbnormalEffectFactory> const>;

public:
    void setType(SkillType type);
    void setTargetType(SkillTargetType type);
    void setTargetNature(SkillTargetNature nature);
    void setIsMagic(bool isMagic);
    void setCastDuration(ClockDuration castDuration);

    template<class T> requires std::is_base_of_v<AbnormalEffectFactory, T>
    void addAbnormalEffectFactory(auto &&... args) {
        _effects.emplace_back(std::make_unique<T>(*this, std::forward<decltype(args)>(args)...));
    }

public:
    void applyEffects(Actor & source, Actor & target) const;

private:
    SkillId           _id;
    SkillLevel        _level;
    SkillType         _type;
    SkillTargetType   _targetType;
    SkillTargetNature _targetNature;
    std::string       _name;
    std::string       _fullName;
    std::string       _icon;
    bool              _isMagic;
    ClockDuration     _castDuration;
    ClockDuration     _cooldownDuration;

    std::vector<std::unique_ptr<AbnormalEffectFactory>> _effects;
};
