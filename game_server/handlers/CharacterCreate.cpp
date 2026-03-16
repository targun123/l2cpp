/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"
#include "../game/components/PlayerAppearance.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(CharacterCreate)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    std::wstring name;
    Race race;
    Sex sex;

    auto & c = player.addCharacter();
    reader
        >> name
        >> race
        >> sex
        >> c.classId
        >> c.baseStats.INT
        >> c.baseStats.STR
        >> c.baseStats.CON
        >> c.baseStats.MEN
        >> c.baseStats.DEX
        >> c.baseStats.WIT
        >> c.appearance().hairStyleId
        >> c.appearance().hairColorId
        >> c.appearance().faceId
    ;

    c.setName(std::move(name));
    c.appearance().setRace(race);
    c.appearance().sex = sex;
    c.selected = 1;
    player.connection().send(Packet(0x19) << 1);
    handleCharacterList(player);
}
