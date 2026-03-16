/// @author    Chnossos
/// @date      Created on 2026-02-24

// Project includes
#include "_Common.hpp"
#include "../game/actor/Character.hpp"

DECLARE_PACKET_HANDLER(CharacterList)

DEFINE_PACKET_HANDLER(CharacterCancelDeletion)
{
    PacketReader reader(player.connection().readBuffer().subspan(3));

    u32 characterId;
    reader >> characterId;

    // ReSharper disable once CppTooWideScopeInitStatement
    auto const it = std::ranges::find_if(player.characters(), [=] (auto const & c) { return c.id() == characterId; });
    if (it != player.characters().cend())
        it->deleteTime = 0;

    handleCharacterList(player);
}
