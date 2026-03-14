/// @author    Chnossos
/// @date      Created on 2026-03-14

#pragma once

// Project includes
#include "Skill.hpp"

#include <l2cpp/Pimpl.hpp>

/// List of skills a (N)PC has learned.
class SkillDirectory
{
public:
    SkillDirectory();
    SkillDirectory(SkillDirectory &&) noexcept;
    SkillDirectory & operator=(SkillDirectory &&) noexcept;
    ~SkillDirectory();

public:
    auto size() const -> size_t;
    auto skill(SkillId id) -> OptionalRef<Skill>;
    auto skill(SkillId id) const -> OptionalRef<Skill const>;

public:
    auto learn(SkillId id, SkillLevel level) -> Skill &;

public:
    auto begin() -> std::unordered_map<SkillId, Skill>::iterator;
    auto end()   -> std::unordered_map<SkillId, Skill>::iterator;

    auto begin() const -> std::unordered_map<SkillId, Skill>::const_iterator;
    auto end()   const -> std::unordered_map<SkillId, Skill>::const_iterator;

    auto cbegin() const -> std::unordered_map<SkillId, Skill>::const_iterator;
    auto cend()   const -> std::unordered_map<SkillId, Skill>::const_iterator;

private:
    struct SkillDirectoryImpl;
    Pimpl<SkillDirectoryImpl> _impl;
};
