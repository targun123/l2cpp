/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

enum class SkillTargetType
{
    None,   ///< No target needed (passive or toggle)
    Self,   ///< Skill effects applied on self
    Single, ///< Skill effects applied on single target (can include self)
    AoE,    ///< Skill effects applied around the target or casting location
};
