/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
#include "../game/actor/Character.hpp"
#include "../game/constants/ItemGrade.hpp"
#include "../game/inventory/Gear.hpp"
#include "../network/packets/server/TargetMonsterSelectPacket.hpp"
#include "../network/packets/server/TargetSelectPacket.hpp"

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
        /**/ if (auto const c = World::character(targetId); c)
        {
            character.setTarget(c->get());
            player.connection().send(TargetSelectPacket(character, c->get()));
        }
        else if (auto const m = World::monster(targetId); m)
        {
            character.setTarget(m->get());
            player.connection().send(TargetMonsterSelectPacket(character, m->get()));
        }
    }
    else // second request on target, launch attack!
    {
        // Enable attack stance on opponents
        player.connection().send(Packet(0x2b) << targetId);
        player.connection().send(Packet(0x2b) << character.id());

        // Attack once, use soulshots if a weapon is equipped
        auto flag = 0_u8;
        if (auto const weapon = character.gear().weapon(); weapon)
            flag |= 0x10 | std::to_underlying(weapon->get().tmplate.grade);

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
            << u16(0) // other hits (e.g. dual fists)
            << character.target()->get().position().x
            << character.target()->get().position().y
            << character.target()->get().position().z
        ;
        player.connection().send(p);
    }
}
