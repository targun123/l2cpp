/// @author    Chnossos
/// @date      Created on 2026-04-21

#pragma once

enum class SkillTargetNature
{
    None        = 0b0000000000000000, ///< Skill doesn't need any target (passive, toggle…)
    Self        = 0b0000000000000001, ///< Target can be self
    Other       = 0b0000000000000010, ///< Target can be anything other than self
    Friendly    = 0b0000000000000100, ///< Target can be friendly (same team, party, clan…)
    Ennemy      = 0b0000000000001000, ///< Target can be an ennemy (opponent, clan war, monster…)
    Monster     = 0b0000000000010000, ///< Target can be a monster
    Character   = 0b0000000000100000, ///< Target can be a character
    PartyMember = 0b0000000001000000, ///< Target can be a party member (friendly or not)
    ClanMember  = 0b0000000010000000, ///< Target can be a clan member (friendly or not)
    Corpse      = 0b0000000100000000, ///< Target can be a corpse
    Undead      = 0b0000001000000000, ///< Target can be of Undead family
    Summon      = 0b0000010000000000, ///< Target can be a summon
    Pet         = 0b0000100000000000, ///< Target can be a pet
    Servitor    = 0b0001000000000000, ///< Target can be a servitor
    SummonOwner = 0b0010000000000000, ///< Target is the targetted summon's owner
    Unlockable  = 0b0100000000000000, ///< Target can be unlockable

    Any         = 0b1111111111111111, ///< Skill can be used regardless of the nature of the target
};
