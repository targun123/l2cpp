/// @author    Chnossos
/// @date      Created on 2026-03-16

#pragma once

// Project includes
#include "Actor.hpp"
#include "../game_server/Player.hpp"
class NpcAppearance;
class NpcStatus;

class Npc : public Actor
{
public:
    explicit Npc(u32 id);

protected:
    Npc(ActorType type, u32 id);

public:
    auto appearance()       -> NpcAppearance       &;
    auto appearance() const -> NpcAppearance const &;
    auto status()     const -> NpcStatus     const &;

    void showChatWindow(Player& player, u32 page);
    void onBypassFeedback(Player& player, std::string command);
    std::wstring html(u32 npcId, u32 page);
    bool isAttackable() const override { return false; }

};
