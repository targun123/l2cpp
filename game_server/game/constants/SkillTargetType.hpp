/// @author    Chnossos
/// @date      Created on 2026-03-11

#pragma once

enum class SkillTargetType
{
    None,   ///< No target needed (passive or toggle or auto-applies to self (e.g. War Cry))
    Single, ///< Skill effects apply on single target (can include self)
    AoE,    ///< Skill effects apply around the target or casting location (can include self)
    Party,  ///< Skill effects apply to the target's party members (can include self)
    Clan,   ///< Skill effects apply to the target's clan members (can include self)
};
