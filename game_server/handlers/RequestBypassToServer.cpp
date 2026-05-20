// Project includes
#include "_Common.hpp"
#include "../network/packets/server/action/ActionFailedPacket.hpp"
#include "../game_server/utils/Conversion.hpp"
#include "../game/World.hpp"
#include "../game_server/game/actor/Character.hpp"
#include "../game_server/utils/Maths.hpp"
#include "../game_server/game/actor/Npc.hpp"
DEFINE_PACKET_HANDLER(RequestBypassToServer) try
{
    L2CPP_B_ASSERT(player.currentCharacter(), "Cannot process bypass, no current character");

    PacketReader reader(player.connection().readBuffer().subspan(3));

    std::wstring bypassWStr;
    reader >> bypassWStr;

    std::string const commandStr = Utils::toString(bypassWStr);

    std::string_view const command = commandStr;

    auto & c = *player.currentCharacter();

    if (command.starts_with("npc_")) {

        size_t endOfId = command.find('_', 4);
        std::string idStr;
        std::string actualBypass = "";

        if (endOfId != std::string::npos) {
            
            idStr = command.substr(4, endOfId - 4);

            actualBypass = command.substr(endOfId + 1);
        }
        else
        {
            idStr = command.substr(4);
        }
       
        if (!idStr.empty() && std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {

            u32 npcObjectId = std::stoul(idStr);
           
            if (auto const targetNpc = c.target()) {

                if (targetNpc->type() == ActorType::Npc && Utils::Maths::isInRange(c, targetNpc, 250.0)) {
                    auto* npc = const_cast<Npc*>(static_cast<Npc const*>(&*targetNpc));

                    npc->onBypassFeedback(*c.player, actualBypass);
                    return;
                }

            }

        }

    }
    player.connection().send(ActionFailedPacket{});
}
catch (...)
{
    player.connection().send(ActionFailedPacket{});
    throw;
}