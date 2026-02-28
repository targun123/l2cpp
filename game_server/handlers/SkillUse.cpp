/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(SkillUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 skillId, ctrlPressed;
    bool shiftPressed;
    reader >> skillId >> ctrlPressed >> shiftPressed;

    auto const & c = player.currentCharacter()->get();
    auto duration  = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(2)).count();

    // Packet p(0x48);
    // p
    //     << c.id     // caster
    //     << c.id     // target
    //     << skillId
    //     << 4        // level
    //     << 1000 // duration
    //     << 10       // reuse delay (in ms?)
    //     << c.pos.x
    //     << c.pos.y
    //     << c.pos.z
    //     << 0        // critical
    //     << c.pos.x
    //     << c.pos.y
    //     << c.pos.z
    // ;

    // Display Super Haste effect in buff bar
    Packet p(0x7f);
    p
        << u16(1)
        << skillId
        << u16(4)
        << duration
    ;
    player.connection().send(p);
}
