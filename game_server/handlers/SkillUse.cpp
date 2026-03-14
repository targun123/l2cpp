/// @author    Chnossos
/// @date      Created on 2026-02-27

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"
#include "../network/packets/server/CharacterStatusUpdatePacket.hpp"

DEFINE_PACKET_HANDLER(SkillUse)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 skillId, ctrlPressed;
    bool shiftPressed;
    reader >> skillId >> ctrlPressed >> shiftPressed;

    auto & c      = player.currentCharacter()->get();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(2)).count();

    // Start spell animation
    {
        Packet p(0x48);
        p
            << c.id()  // caster
            << c.id()  // target
            << 78
            << 2       // level
            << 1500    // duration
            << 180000  // reuse delay (in ms?)
            << c.pos.x
            << c.pos.y
            << c.pos.z
            << 0       // critical
            << c.pos.x
            << c.pos.y
            << c.pos.z
        ;
        player.connection().send(p);
    }

    // Display the cast bar?
    {
        Packet p(0x76);
        p
            << c.id()
            << 78
            << 2
            << 1
            << c.id()
        ;
        player.connection().send(p);
    }

    static bool toggle = true;

    if (toggle)
    {
        // Super Haste lv. 4 effect on move speed
        c.finalStats.moveSpeedMutliplier += 5;

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
    else
    {
        c.finalStats.moveSpeedMutliplier -= 5;
        player.connection().send(Packet(0x7f) << 0); // Remove from bar
    }
    toggle = !toggle;

    // Update status info
    player.connection().send(CharacterStatusUpdatePacket(c));
}
