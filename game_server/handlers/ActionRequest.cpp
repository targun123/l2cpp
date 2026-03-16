/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"

DEFINE_PACKET_HANDLER(ActionRequest)
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot select target, no current character");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 targetId;
    s32 originX, originY, originZ;
    u8 actionId; // 0=click 1=shift+click
    reader >> targetId >> originX >> originY >> originZ >> actionId;

    auto & character = player.currentCharacter()->get();

    // No current target or current target is different?
    if (!character.target() || character.target()->get().id() != targetId)
    {
        /**/ if (auto const c = World::character(targetId); c) character.setTarget(c->get());
        else if (auto const m = World::monster(targetId);   m) character.setTarget(m->get());
        else                                                   return;

        player.connection().send(Packet(0xa6) << targetId << static_cast<u16>(-10));
    }
    else // second request on target, launch attack!
    {
        // Enable attack stance on opponents
        player.connection().send(Packet(0x2b) << targetId);
        player.connection().send(Packet(0x2b) << character.id());

        enum class ItemGrade
        {
            NoGrade = 0b0000'0000,
            D       = 0b0000'0001,
            C       = 0b0000'0010,
            B       = 0b0000'0011,
            A       = 0b0000'0100,
            S       = 0b0000'0101,
        };

        // Attack once
        u8 const flag = 0x10 | std::to_underlying(ItemGrade::S);

        Packet p(0x05);
        p
            << character.id()
            // Hit
            << targetId
            << 10 // dmg
            << flag  // flags: 0x10=use_ss 0x20=crit_sound 0x40=shield_sound 0x80=miss_sound
            << character.position().x
            << character.position().y
            << character.position().z
            << u16(0) // other hits
            << character.target()->get().position().x
            << character.target()->get().position().y
            << character.target()->get().position().z
        ;
        player.connection().send(p);
    }
}
