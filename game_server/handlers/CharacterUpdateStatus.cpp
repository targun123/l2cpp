/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(CharacterUpdateStatus)
{
    auto & c = player.currentCharacter().value().get();

    Packet p(0x04);
    p
        << c.pos.x
        << c.pos.y
        << c.pos.z
        << 0 // head angle
        << c.id
        << c.name
        << c.raceId
        << c.sex
        << c.classId
        << c.level
        << c.xp
        << c.attributes.STR
        << c.attributes.DEX
        << c.attributes.CON
        << c.attributes.INT
        << c.attributes.WIT
        << c.attributes.MEN
        << static_cast<u32>(c.hp.max)
        << static_cast<u32>(c.hp.current)
        << static_cast<u32>(c.mp.max)
        << static_cast<u32>(c.mp.current)
        << c.sp
        << c.weight.current
        << c.weight.max
        << 0x28 // ?
        << std::array<u32, 32>{} // gear
        << c.stats.pAtk
        << c.stats.pAtkSpeed
        << c.stats.pDef
        << c.stats.evasion
        << c.stats.accuracy
        << c.stats.critRate
        << c.stats.mAtk
        << c.stats.mAtkSpeed
        << c.stats.pAtkSpeed
        << c.stats.mDef
        << (c.isPvpFlagged ? 1 : 0)
        << c.karma
        << c.stats.runSpeed
        << c.stats.walkSpeed
        << c.stats.swimRunSpeed
        << c.stats.swimWalkSpeed
        << c.stats.flyRunSpeed
        << c.stats.flyWalkSpeed
        << c.stats.flyRunSpeed
        << c.stats.flyWalkSpeed
        << 1.0 // movement_speed_multiplier,
        << 1.0 // atk_speed_multiplier,
        << c.collisionRadius
        << c.collisionHeight
        << c.hairStyleId
        << c.hairColorId
        << c.faceId
        << (c.accessLevel > 0 ? 1 : 0)
        << c.title
        << c.clanId
        << 0 // crest id
        << 0 // ally id
        << 0 // ally crest id
        << 0 // clan leader: 0x60 | in-siege: 0x40 | leader rights: 0x20
        << static_cast<u8>(0) // mount type
        << static_cast<u8>(0) // private store type
        << false // can dwarven craft
        << c.pkCount
        << c.pvpCount
    ;

    p << static_cast<u16>(c.cubics.size());
    for (auto id : c.cubics)
        p << id;

    p
        << false // looking for party members
        << 0 // abnormal effects
        << static_cast<u8>(0) // ?
        << 0 // clan privileges
        << 0 // 0x100 // swim
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << 0 // ?
        << c.evalAmount
        << c.evalScore
        << 0 // ?
        << c.inventoryLimit
        << c.classId
        << 0 // special effects? circles around player...
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << c.enchantEffect
        << c.team // duel team color: 0=none 1=blue 2=red
        << 0 // clan crest large id
        << c.isHero // hero symbol in status window
        << c.isHero // hero aura
        << static_cast<u8>(0) // fishing mode (unused)
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.nameColor
    ;

    player.connection().send(p);
}
