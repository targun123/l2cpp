/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "SkillTemplate.hpp"

SkillTemplate::SkillTemplate(SkillId const id, std::string name, SkillLevel const lvl)
    : _id(id)
    , _level(lvl)
    , _name(std::move(name))
    , _fullName(std::format("{} lv. {}", _name, _level))
    , _icon(std::format("icon.skill{:04}", _id))
{}

SkillTemplate::SkillTemplate(
    SkillId         const id
  , std::string           name
  , SkillLevel      const lvl
  , SkillType       const type
  , SkillTargetType const targetType
  , MSec            const castDuration
  , MSec            const cdDuration
)
    : _id(id)
    , _level(lvl)
    , _type(type)
    , _targetType(targetType)
    , _name(std::move(name))
    , _fullName(std::format("{} lv. {}", _name, _level))
    , _icon(std::format("icon.skill{:04}", _id))
    , _castDuration(castDuration)
    , _cooldownDuration(cdDuration)
{}

auto SkillTemplate::uid()          const -> SkillUid         { return {_id, _level}; }
auto SkillTemplate::id()           const -> SkillId          { return _id;           }
auto SkillTemplate::name()         const -> std::string_view { return _name;         }
auto SkillTemplate::fullName()     const -> std::string_view { return _fullName;     }
auto SkillTemplate::level()        const -> SkillLevel       { return _level;        }
auto SkillTemplate::type()         const -> SkillType        { return _type;         }
auto SkillTemplate::castDuration() const -> MSec             { return _castDuration; }

auto SkillTemplate::effects() const -> std::span<AbnormalEffectType const> { return _effects; }

void SkillTemplate::setCastDuration(MSec const castDuration) { _castDuration = castDuration; }
void SkillTemplate::setType(SkillType const type)            { _type = type;                 }

void SkillTemplate::setEffectTypes(std::vector<AbnormalEffectType> effects) { _effects = std::move(effects); }
