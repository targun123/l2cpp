#pragma once
#include "../IBypassHandler.hpp"
#include <vector>
#include <string_view>

class ChatLink : public IBypassHandler
{
public:
    ChatLink() = default;
    ~ChatLink() override = default;

    auto onCommand(std::string_view command, Player& player, Actor& target) -> bool override;
    auto CommandList() const -> std::vector<std::string_view> override;
};