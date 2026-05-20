/// @author    Chnossos
/// @date      Created on 2026-03-16

#include "Npc.hpp"

// Project includes
#include "../components/NpcAppearance.hpp"
#include "../components/NpcStatus.hpp"
#include "../../Player.hpp"
#include "../game_server/network/packets/server/action/NpcHtmlMessagePacket.hpp"
#include "../game_server/network/Connection.hpp"
#include "../game_server/game/actions/bypass/BypassHandler.hpp"
#include "NpcDirectory.hpp"
#include <spdlog/spdlog.h>

Npc::Npc(u32 const id)
    : Npc(ActorType::Npc, id)
{}

Npc::Npc(ActorType const type, u32 const id)
    : Actor(type)
{
    addComponent<NpcAppearance>().setId(id);
    addComponent<NpcStatus>();
}

void Npc::onBypassFeedback(Player& player, std::string command) {

	if (auto const handler = BypassHandler::instance().Handler(command))
	{

		handler->onCommand(command, player, *this);
	}
	else
	{
		
		SPDLOG_INFO("Npc: Unknown NPC bypass: \"{}\" NpcId: {}", command, this->id());
	}

}

void Npc::showChatWindow(Player& player, u32 page) {
	u32 npcId = appearance().templateId();
	u32 objectId = appearance().id();
	std::wstring htmlContent = html(npcId, page);

	std::wstring from = L"%objectId%";
	std::wstring to = std::to_wstring(objectId);

	size_t start_pos = 0;

	while ((start_pos = htmlContent.find(from, start_pos)) != std::wstring::npos) {
		htmlContent.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	player.connection().send(Network::Packet::Server::NpcHtmlMessagePacket(objectId, htmlContent));



}

std::wstring Npc::html(u32 npcId, u32 page)
{
	std::wstring htmlContent = NpcDirectory::html(npcId, page);
	return htmlContent;
}

auto Npc::appearance()       -> NpcAppearance       & { return component<NpcAppearance>(); }
auto Npc::appearance() const -> NpcAppearance const & { return component<NpcAppearance>(); }
auto Npc::status()     const -> NpcStatus     const & { return component<NpcStatus>();     }

