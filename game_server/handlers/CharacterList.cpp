/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DEFINE_PACKET_HANDLER(CharacterList)
{
    Packet p(0x13);
    p << static_cast<u32>(player.characters().size());

    for (auto const & c : player.characters())
    {
        p
            << c.name
            << c.id
            << player.accountName()
            << player.playOk1()
            << c.clanId
            << 0
            << c.sex
            << c.raceId
            << c.classId
            << 1 // active
            << c.pos.x
            << c.pos.y
            << c.pos.z
            << c.hp.current
            << c.mp.current
            << c.sp
            << c.xp
            << c.level
            << c.karma
            << std::array<u32, 9>{} // unknown
        ;

        using enum l2::InventoryGearSlot;
        p
            << c.equippedItem(Underwear   ).uid
            << c.equippedItem(LeftEarring ).uid
            << c.equippedItem(RightEarring).uid
            << c.equippedItem(Necklace    ).uid
            << c.equippedItem(LeftRing    ).uid
            << c.equippedItem(RightRing   ).uid
            << c.equippedItem(Helmet      ).uid
            << c.equippedItem(RightWeapon ).uid
            << c.equippedItem(Shield      ).uid
            << c.equippedItem(Gloves      ).uid
            << c.equippedItem(UpperArmor  ).uid
            << c.equippedItem(LowerArmor  ).uid
            << c.equippedItem(Feet        ).uid
            << c.equippedItem(Cloack      ).uid
            << c.equippedItem(RightWeapon ).uid // Dual weapons duplicate right hand into left hand
            << c.equippedItem(Hair        ).uid
            << c.equippedItem(Underwear   ).tmplate.id
            << c.equippedItem(LeftEarring ).tmplate.id
            << c.equippedItem(RightEarring).tmplate.id
            << c.equippedItem(Necklace    ).tmplate.id
            << c.equippedItem(LeftRing    ).tmplate.id
            << c.equippedItem(RightRing   ).tmplate.id
            << c.equippedItem(Helmet      ).tmplate.id
            << c.equippedItem(RightWeapon ).tmplate.id
            << c.equippedItem(Shield      ).tmplate.id
            << c.equippedItem(Gloves      ).tmplate.id
            << c.equippedItem(UpperArmor  ).tmplate.id
            << c.equippedItem(LowerArmor  ).tmplate.id
            << c.equippedItem(Feet        ).tmplate.id
            << c.equippedItem(Cloack      ).tmplate.id
            << c.equippedItem(RightWeapon ).tmplate.id // Dual weapons duplicate right hand into left hand
            << c.equippedItem(Hair        ).tmplate.id
        ;

        p
            << c.hairStyleId
            << c.hairColorId
            << c.faceId
            << c.hp.max
            << c.mp.max
            << c.deleteTime
            << c.classId
            << c.selected
            << static_cast<u8>(std::min<u16>(c.equippedItem(RightWeapon).enchantLevel, 127)) // Visually enable max glow
        ;
    }

    player.connection().send(p);
}
