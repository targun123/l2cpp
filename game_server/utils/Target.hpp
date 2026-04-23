/// @author    Chnossos
/// @date      Created on 2026-04-22

#pragma once

class Actor;
class SkillTemplate;

namespace Utils::Target
{
    bool isValidTarget(Actor const & emitter, SkillTemplate const & skill, Actor const & target, bool forceAttack);
}
