/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "SkillInfo.hpp"

SkillInfo::SkillInfo(SkillId const id, std::string name, SkillLevel const lvl)
    : _id(id)
    , _level(lvl)
    , _name(std::move(name))
    , _fullName(std::format("{} lv. {}", _name, _level))
    , _icon(std::format("icon.skill{:04}", _id))
{}

SkillInfo::SkillInfo(
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

auto SkillInfo::uid() const -> SkillUid
{
    return {_id, _level};
}

auto SkillInfo::id() const -> SkillId
{
    return _id;
}

auto SkillInfo::name() const -> std::string_view
{
    return _name;
}

auto SkillInfo::fullName() const -> std::string_view
{
    return _fullName;
}

auto SkillInfo::level() const -> SkillLevel
{
    return _level;
}

auto SkillInfo::type() const -> SkillType
{
    return _type;
}

void SkillInfo::setCastDuration(MSec const castDuration)
{
    _castDuration = castDuration;
}
