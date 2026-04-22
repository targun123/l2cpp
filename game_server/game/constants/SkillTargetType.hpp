/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

enum class SkillTargetType
{
    Self,   ///< Skill effects apply to self only
    Single, ///< Skill effects apply on single target
    Aura,   ///< Skill effects apply around self @note Can include self if @c SkillTargetNature includes @c Self
    AoE,    ///< Skill effects apply around the target
};
