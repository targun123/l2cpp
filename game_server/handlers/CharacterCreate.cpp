/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/Character.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(CharacterCreate)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 ignore;

    auto & c = player.addCharacter();
    reader >> c.name >> c.raceId >> c.sex >> c.classId >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
           >> c.hairStyleId >> c.hairColorId >> c.faceId;
    c.selected = 1;

    player.connection().send(Packet(0x19) << 1);
    handleCharacterList(player);
}
