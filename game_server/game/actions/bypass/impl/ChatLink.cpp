#include "ChatLink.hpp"
#include <charconv> 

#include "../game_server/Player.hpp"
#include "../game_server/game/actor/Npc.hpp" 

auto ChatLink::CommandList() const -> std::vector<std::string_view>
{
    return { "Chat" };
}

auto ChatLink::onCommand(std::string_view command, Player& player, Actor& target) -> bool
{
    
    if (target.type() != ActorType::Npc)
    {
        return false;
    }

    int val = 0;

   
    if (command.length() > 5)
    {
        std::string_view const numberPart = command.substr(5);

        std::from_chars(numberPart.data(), numberPart.data() + numberPart.size(), val);
    }

    
    auto* npc = static_cast<Npc*>(&target);

    
    if (val == 0 )
    {
        
        npc->showChatWindow(player, 0);
    }
    else
    {
        
        npc->showChatWindow(player, val);
    }

    return false;
}