/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/World.hpp"
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
    u32 INT, STR, CON, MEN, DEX, WIT;

    auto & c     = World::addCharacterPreview(player.accountName());
    auto & stats = c.stats();

    reader
        >> name
        >> race
        >> sex
        >> profession
        >> INT
        >> STR
        >> CON
        >> MEN
        >> DEX
        >> WIT
        >> c.appearance().hairStyleId
        >> c.appearance().hairColorId
        >> c.appearance().faceId
    ;

    stats[StatId::Int] = INT;
    stats[StatId::Str] = STR;
    stats[StatId::Con] = CON;
    stats[StatId::Men] = MEN;
    stats[StatId::Dex] = DEX;
    stats[StatId::Wit] = WIT;

    c.setName(std::move(name));
    c.appearance().setRace(race);
    c.appearance().sex = sex;
    c.setProfession(profession);
    c.selected = 1;

    player.connection().send(Packet(0x19) << 1);
    handleCharacterList(player);
}
