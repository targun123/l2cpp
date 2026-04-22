/// @author    Chnossos
/// @date      Created on 2026-04-22

#include "Target.hpp"

// Project includes
#include "../game/actor/Actor.hpp"
#include "../game/skill/SkillTemplate.hpp"

#include <l2cpp/utils/EnumMask.hpp>

bool Utils::Target::isValidTarget(Actor const & emitter, SkillTemplate const & skill,
                                  Actor const & target, bool const forceAttack)
{
    using enum SkillTargetNature;

    EnumMask const targetNature = skill.targetNature();

    if (targetNature == None) [[unlikely]]
        return false;

    if (target == emitter)
        return targetNature & Self;

    if (!target.isAlive())
    {
        switch (target.type())
        {
            case ActorType::Character: return targetNature & Character; // e.g. Ressurection      is Corpse | Character
            case ActorType::Monster:   return targetNature & Monster;   // e.g. Corpse Life Drain is Corpse | Monster
            case ActorType::Npc:       return targetNature & Npc;       // e.g. NPC Corpse Remove is Corpse | Npc
        }
    }

    switch (target.type())
    {
        case ActorType::Npc:
        case ActorType::Character:
            return targetNature & Friendly || forceAttack;

        case ActorType::Monster:
            return targetNature & Ennemy;
    }

    return false;
}
