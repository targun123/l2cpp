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
            << c.id()
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

        auto const equippedItemIdIfValid = [&c] (l2::InventoryGearSlot const s)
        {
            auto const item = c.equippedItem(s);
            return item ? item->get().id() : 0;
        };

        auto const equippedItemTemplateIdIfValid = [&c] (l2::InventoryGearSlot const s)
        {
            auto const item = c.equippedItem(s);
            return item ? item->get().tmplate.id : 0;
        };

        using enum l2::InventoryGearSlot;
        p
            << equippedItemIdIfValid(Underwear   )
            << equippedItemIdIfValid(LeftEarring )
            << equippedItemIdIfValid(RightEarring)
            << equippedItemIdIfValid(Necklace    )
            << equippedItemIdIfValid(LeftRing    )
            << equippedItemIdIfValid(RightRing   )
            << equippedItemIdIfValid(Helmet      )
            << equippedItemIdIfValid(RightWeapon )
            << equippedItemIdIfValid(Shield      )
            << equippedItemIdIfValid(Gloves      )
            << equippedItemIdIfValid(UpperArmor  )
            << equippedItemIdIfValid(LowerArmor  )
            << equippedItemIdIfValid(Feet        )
            << equippedItemIdIfValid(Cloack      )
            << equippedItemIdIfValid(RightWeapon ) // Dual weapons duplicate right hand into left hand
            << equippedItemIdIfValid(Hair        )
            << equippedItemTemplateIdIfValid(Underwear   )
            << equippedItemTemplateIdIfValid(LeftEarring )
            << equippedItemTemplateIdIfValid(RightEarring)
            << equippedItemTemplateIdIfValid(Necklace    )
            << equippedItemTemplateIdIfValid(LeftRing    )
            << equippedItemTemplateIdIfValid(RightRing   )
            << equippedItemTemplateIdIfValid(Helmet      )
            << equippedItemTemplateIdIfValid(RightWeapon )
            << equippedItemTemplateIdIfValid(Shield      )
            << equippedItemTemplateIdIfValid(Gloves      )
            << equippedItemTemplateIdIfValid(UpperArmor  )
            << equippedItemTemplateIdIfValid(LowerArmor  )
            << equippedItemTemplateIdIfValid(Feet        )
            << equippedItemTemplateIdIfValid(Cloack      )
            << equippedItemTemplateIdIfValid(RightWeapon ) // Dual weapons duplicate right hand into left hand
            << equippedItemTemplateIdIfValid(Hair        )
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
        ;

        if (auto const item = c.equippedItem(RightWeapon); item)
            p << item->get().enchantLevel;
        else
            p << 0_u8;
    }

    player.connection().send(p);
}
