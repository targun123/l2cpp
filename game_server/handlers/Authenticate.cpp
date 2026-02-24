/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(Authenticate)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    std::wstring accountName;
    u32 playOk1, playOk2, loginOk1, loginOk2;
    reader >> accountName >> playOk2 >> playOk1 >> loginOk1 >> loginOk2;

    player.setAccountName(std::move(accountName));
    player.setPlayOk1(playOk1);

    handleCharacterList(player);
}
