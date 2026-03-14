/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

// Project includes
#include "SkillInfo.hpp"

#include <l2cpp/Pimpl.hpp>

// C++ includes
#include <unordered_map>

class Skill
{
public:
    explicit Skill(SkillInfo info);
    Skill(Skill &&) noexcept;
    Skill & operator=(Skill &&) noexcept;
    ~Skill();

public:
    auto info() const -> SkillInfo const &;

private:
    struct SkillImpl;
    Pimpl<SkillImpl> _impl;
};

using SkillRegistry = std::unordered_map<SkillUid, Skill>;
