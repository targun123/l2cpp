/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/Position.hpp"
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

    Packet p(0x48);
    p
        << c.id()  // caster
        << (c.target() ? c.target()->id() : c.id())
        << static_cast<u32>(skill->tmplate().id())
        << static_cast<u32>(skill->tmplate().level())
        << static_cast<u32>(skill->tmplate().castDuration().count())
        << 1000 // reuse delay (in ms)
        << c.position().x
        << c.position().y
        << c.position().z
        << 0 // critical
        << c.position().x
        << c.position().y
        << c.position().z
    ;
    player.connection().send(p);
}
