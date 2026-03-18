/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/Stats.hpp"
#include "../game/components/PlayerAppearance.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(CharacterCreate)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    std::wstring name;
    Race race;
    Sex sex;
    Profession profession;

    auto & c         = player.addCharacter();
    auto & baseStats = c.component<Stats>();

    reader
        >> name
        >> race
        >> sex
        >> profession
        >> baseStats.INT
        >> baseStats.STR
        >> baseStats.CON
        >> baseStats.MEN
        >> baseStats.DEX
        >> baseStats.WIT
        >> c.appearance().hairStyleId
        >> c.appearance().hairColorId
        >> c.appearance().faceId
    ;

    c.setName(std::move(name));
    c.appearance().setRace(race);
    c.appearance().sex = sex;
    c.selected = 1;
    c.setProfession(profession);

    player.connection().send(Packet(0x19) << 1);
    handleCharacterList(player);
}
