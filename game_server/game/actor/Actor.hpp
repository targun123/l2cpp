/// @author    Chnossos
/// @date      Created on 2026-03-15

#pragma once

// Project includes
#include "../GameObject.hpp"
#include "../ecs/Entity.hpp"
#include "../components/Position.hpp"
#include "../constants/Team.hpp"

#include <l2cpp/Pimpl.hpp>

struct ComputedStats;
struct Stats;

/// Base class for player characters, npcs, summons, pets…
class Actor : public GameObject, public Entity
{
public:
    Actor();
    Actor(Actor &&) noexcept;
    Actor & operator=(Actor &&) noexcept;
    ~Actor() override = 0;

public:
    auto name() const -> std::wstring_view;
    auto title() const -> std::wstring_view;
    auto position() const -> Position const &;
    auto team() const -> Team;

    auto baseStats() const -> Stats const &;
    auto stats() const -> ComputedStats const &;

public:
    void setName(std::wstring name);
    void setTitle(std::wstring title);
    void setPosition(s32 x, s32 y, s32 z);
    void setPosX(s32 x);
    void setPosY(s32 y);
    void setPosZ(s32 z);
    void setTeam(Team team);

private:
    struct ActorImpl;
    Pimpl<ActorImpl> _impl;
};
