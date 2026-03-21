/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/CharacterStatus.hpp"
#include "../game/components/Gear.hpp"
#include "../game/components/PlayerAppearance.hpp"
#include "../game/components/Position.hpp"

DEFINE_PACKET_HANDLER(CharacterList)
{
    Packet p(0x13);
    p << static_cast<u32>(player.characters().size());

    using enum GearSlot;
    for (auto const & ref : player.characters())
    {
        auto const & c = ref.get();

        p
            << c.name()
            << c.id()
            << player.accountName()
            << player.playOk1()
            << 0 // clanId
            << 0 // ?
            << c.appearance().sex
            << c.appearance().race()
            << c.profession()
            << 1 // active (?)
            << c.position().x
            << c.position().y
            << c.position().z
            << c.status().hp.current
            << c.status().mp.current
            << c.status().sp
            << c.status().xp
            << c.status().level()
            << c.status().karma
            << std::array<u32, 9>{} // unknown
            << c.gear().itemId(Underwear)
            << c.gear().itemId(LeftEar)
            << c.gear().itemId(RightEar)
            << c.gear().itemId(Neck)
            << c.gear().itemId(LeftFinger)
            << c.gear().itemId(RightFinger)
            << c.gear().itemId(Head)
            << c.gear().itemId(RightHand)
            << c.gear().itemId(LeftHand)
            << c.gear().itemId(Gloves)
            << c.gear().itemId(Chest)
            << c.gear().itemId(Legs)
            << c.gear().itemId(Feet)
            << c.gear().itemId(Back)
            << 0 // c.gear().itemId(LeftHand)
            << c.gear().itemId(Hair)
            << c.gear().itemTemplateId(Underwear)
            << c.gear().itemTemplateId(LeftEar)
            << c.gear().itemTemplateId(RightEar)
            << c.gear().itemTemplateId(Neck)
            << c.gear().itemTemplateId(LeftFinger)
            << c.gear().itemTemplateId(RightFinger)
            << c.gear().itemTemplateId(Head)
            << c.gear().itemTemplateId(RightHand)
            << c.gear().itemTemplateId(LeftHand)
            << c.gear().itemTemplateId(Gloves)
            << c.gear().itemTemplateId(Chest)
            << c.gear().itemTemplateId(Legs)
            << c.gear().itemTemplateId(Feet)
            << c.gear().itemTemplateId(Back)
            << 0 // c.gear().itemTemplateId(LeftHand)
            << c.gear().itemTemplateId(Hair)
            << c.appearance().hairStyleId
            << c.appearance().hairColorId
            << c.appearance().faceId
            << c.status().hp.max
            << c.status().mp.max
            << c.deleteTime
            << c.profession()
            << c.selected
        ;

        auto const weapon = c.gear().weapon();
        p << (weapon ? weapon->get().enchantLevel : 0_u8);
    }

    player.connection().send(p);
}
