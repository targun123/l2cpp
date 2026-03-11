/// @author    Chnossos
/// @date      Created on 2026-03-11

#include "Skill.hpp"

// Project includes
#include <l2cpp/details/Pimpl.hpp>

struct Skill::SkillImpl
{
    SkillId         id         = 0;
    SkillLevel      level      = 0;
    SkillType       type       = SkillType::Unknown;
    SkillTargetType targetType = SkillTargetType::None;
    std::string     name, fullName, icon;

    SkillImpl() = default;
    SkillImpl(SkillId const id, std::string name, SkillLevel const level,
              SkillType const type, SkillTargetType const targetType)
        : id(id), level(level), type(type), targetType(targetType), name(std::move(name))
    {
        fullName = std::format("{} lv. {}", name, level);
        icon     = std::format("icon.skill{:04}", id);
    }
};

template class Pimpl<Skill::SkillImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Skill::Skill(SkillId id, std::string name, SkillLevel level, SkillType type, SkillTargetType targetType)
    : _impl(id, std::move(name), level, type, targetType)
{}

Skill::Skill(Skill &&) noexcept = default;
Skill & Skill::operator=(Skill &&) noexcept = default;
Skill::~Skill() = default;

auto Skill::uid() const -> SkillUid
{
    return {_impl->id, _impl->level};
}

auto Skill::id() const -> SkillId
{
    return _impl->id;
}

auto Skill::name() const -> std::string_view
{
    return _impl->name;
}

auto Skill::fullName() const -> std::string_view
{
    return _impl->fullName;
}

auto Skill::level() const -> SkillLevel
{
    return _impl->level;
}

auto Skill::type() const -> SkillType
{
    return _impl->type;
}
