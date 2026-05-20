#include "QuestLink.hpp"
#include <charconv> 

#include "../game_server/Player.hpp"
#include "../game_server/game/actor/Npc.hpp" 
#include <spdlog/spdlog.h>

auto QuestLink::CommandList() const -> std::vector<std::string_view>
{
    return { "Quest" };
}

auto QuestLink::onCommand(std::string_view command, Player& player, Actor& target) -> bool
{

   
    SPDLOG_INFO("TODO: QuestLink class");


   

    return false;
}