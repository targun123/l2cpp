/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/actions/SkillAction.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/SkillDirectory.hpp"

DEFINE_PACKET_HANDLER(SkillUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 skillId, ctrlPressed;
    bool shiftPressed;
    reader >> skillId >> ctrlPressed >> shiftPressed;

    auto & c         = *player.currentCharacter();
    auto const skill = c.skills().skill(static_cast<SkillId>(skillId));
    L2CPP_B_ASSERT(skill, "Character does not possess skill id '{}'", skillId);

    // TODO: check that the skill can actually be casted here (enough MP, relevant target if required, etc.)

    c.doNext<SkillAction>(skill);
}
