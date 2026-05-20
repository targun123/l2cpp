#pragma once
#pragma once
#include <string_view>
#include <vector>
#include <memory>

class Player;
class Actor;

class IBypassHandler
{
public:
    virtual ~IBypassHandler() = default;

    virtual auto onCommand(std::string_view command, Player& player, Actor& bypassOrigin) -> bool = 0;

    virtual auto CommandList() const -> std::vector<std::string_view> = 0;
};