/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

enum class SkillTargetType
{
    Self,   ///< Skill effects apply to self
    Single, ///< Skill effects apply to the target
    Aura,   ///< Skill effects apply around self @note Can include self if @c SkillTargetNature includes @c Self
    AoE,    ///< Skill effects apply to and around the target
};
