/// @author    Chnossos
/// @date      Created on 2026-03-13

#pragma once

// Project includes
#include "SkillInfo.hpp"

// C++ includes/
#include <filesystem>
#include <unordered_map>

class SkillInfoRegister
{
private:
    SkillInfoRegister() noexcept = default;

public:
    static void load(std::filesystem::path const & skillNamesFile, std::filesystem::path const & skillGroupsFile);

public:
    static auto size() -> size_t;
    static auto skill(SkillUid uid) -> OptionalRef<SkillInfo>;
    static auto skill(SkillId const id, SkillLevel const lvl) -> OptionalRef<SkillInfo> { return skill({id, lvl}); }

private:
    static std::unordered_map<SkillUid, SkillInfo> _infos;
};
