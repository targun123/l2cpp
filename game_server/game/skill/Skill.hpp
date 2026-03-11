/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "SkillType.hpp"
#include "SkillTargetType.hpp"
#include "SkillUid.hpp"

#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

// C++ includes
#include <format>
#include <string>
#include <unordered_map>

class Skill
{
public:
    Skill(SkillId id, std::string name, SkillLevel level, SkillType type, SkillTargetType targetType);
    Skill(Skill &&) noexcept;
    Skill & operator=(Skill &&) noexcept;
    ~Skill();

public:
    auto uid()      const -> SkillUid;
    auto id()       const -> SkillId;
    auto name()     const -> std::string_view;
    auto fullName() const -> std::string_view;
    auto level()    const -> SkillLevel;
    auto type()     const -> SkillType;

private:
    struct SkillImpl;
    Pimpl<SkillImpl> _impl;
};

using SkillRegistry = std::unordered_map<SkillUid, Skill>;
