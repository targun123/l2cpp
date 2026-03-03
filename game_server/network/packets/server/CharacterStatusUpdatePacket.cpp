/// @author    Chnossos
/// @date      Created on 2026-02-28

#include "CharacterStatusUpdatePacket.hpp"

// Project includes
#include "../../../game/Character.hpp"

// C++ includes
#include <array>

using namespace Network::Packet::Server;

CharacterStatusUpdatePacket::CharacterStatusUpdatePacket(l2::Character & c)
    : Packet(0x04)
{
    *this
        << c.pos.x
        << c.pos.y
        << c.pos.z
        << c.headAngle
        << c.id
        << c.name
        << c.raceId
        << c.sex
        << c.classId
        << c.level
        << c.xp
        << c.finalStats.STR
        << c.finalStats.DEX
        << c.finalStats.CON
        << c.finalStats.INT
        << c.finalStats.WIT
        << c.finalStats.MEN
        << static_cast<u32>(c.hp.max)
        << static_cast<u32>(c.hp.current)
        << static_cast<u32>(c.mp.max)
        << static_cast<u32>(c.mp.current)
        << c.sp
        << c.weight.current
        << c.weight.max
        << 20 // 20 no weapon, 40 weapon equipped
        << std::array<u32, 32>{} // gear
        << c.finalStats.pAtk
        << c.finalStats.pAtkSpeed
        << c.finalStats.pDef
        << c.finalStats.evasion
        << c.finalStats.accuracy
        << c.finalStats.pCritRate
        << c.finalStats.mAtk
        << c.finalStats.mAtkSpeed
        << c.baseStats.pAtkSpeed
        << c.finalStats.mDef
        << (c.isPvpFlagged ? 1 : 0)
        << c.karma
        << c.baseStats.runSpeed
        << c.baseStats.walkSpeed
        << c.baseStats.swimRunSpeed
        << c.baseStats.swimWalkSpeed
        << c.baseStats.flyRunSpeed
        << c.baseStats.flyWalkSpeed
        << c.baseStats.flyRunSpeed
        << c.baseStats.flyWalkSpeed
        << c.finalStats.moveSpeedMutliplier
        << c.finalStats.pAtkSpeedMutliplier
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

    *this << static_cast<u16>(c.cubics.size());
    for (auto id : c.cubics)
        *this << id;

    *this
        << false // looking for party members
        << 0     // abnormal effects
        << false // is inside water
        << 0     // clan privileges
        << 0     // 0x100 - swim?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << 0     // ?
        << c.evalAmount
        << c.evalScore
        << 0 // ?
        << c.inventoryLimit
        << c.classId
        << 0 // special effects? circles around player...
        << static_cast<u32>(c.cp.max)
        << static_cast<u32>(c.cp.current)
        << c.enchantEffect
        << c.team
        << 0 // clan crest large id
        << c.isNoble // noble symbol in status window
        << c.isHero  // hero aura
        << false // is fishing
        << 0 // fish x
        << 0 // fish y
        << 0 // fish z
        << c.nameColor
    ;
}
