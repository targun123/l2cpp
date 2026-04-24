/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "../game/actions/SkillAction.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/SkillDirectory.hpp"
#include "../network/packets/server/ActionFailedPacket.hpp"
#include "../utils/Target.hpp"
#include "_Common.hpp"

DEFINE_PACKET_HANDLER(SkillUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 skillId, forceAttack;
    bool disallowMovement;
    reader >> skillId >> forceAttack >> disallowMovement;

    auto & c         = *player.currentCharacter();
    auto const skill = c.skills().skill(static_cast<SkillId>(skillId));
    L2CPP_B_ASSERT(skill, "Character does not possess skill id '{}'", skillId);

    auto const target = c.target();

    if (bool canCast = c.isAlive() && (target || !skill->tmplate().needsTarget()))
    {
        if (skill->tmplate().needsTarget())
            canCast = Utils::Target::isValidTarget(c, skill->tmplate(), target, forceAttack);

        if (canCast)
            return c.doNext<SkillAction>(skill->tmplate(), forceAttack);
    }

    player.connection().send(ActionFailedPacket{});
}
