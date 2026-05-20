#pragma once
#include "../IBypassHandler.hpp"
#include <vector>
#include <string_view>

class QuestLink : public IBypassHandler
{
public:
    QuestLink() = default;
    ~QuestLink() override = default;

    auto onCommand(std::string_view command, Player& player, Actor& target) -> bool override;
    auto CommandList() const -> std::vector<std::string_view> override;
};