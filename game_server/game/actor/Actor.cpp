/// @author    Chnossos
/// @date      Created on 2026-03-15

#include "Actor.hpp"

// Project includes
#include "../components/ActorIdentity.hpp"
#include "../components/Position.hpp"

#include <l2cpp/details/Pimpl.hpp>

struct Actor::ActorImpl
{
    Team team = Team::None;
};

template class Pimpl<Actor::ActorImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Actor::Actor()
{
    addComponent<ActorIdentity>();
    addComponent<Position>();
}

Actor::Actor(Actor &&) noexcept = default;
Actor & Actor::operator=(Actor &&) noexcept = default;
Actor::~Actor() = default;

auto Actor::name()     const -> std::wstring_view { return component<ActorIdentity>().name;  }
auto Actor::title()    const -> std::wstring_view { return component<ActorIdentity>().title; }
auto Actor::position() const -> Position const &  { return component<Position>();            }
auto Actor::team()     const -> Team              { return _impl->team;                      }

void Actor::setName (std::wstring name)  { component<ActorIdentity>().name  = std::move(name);  }
void Actor::setTitle(std::wstring title) { component<ActorIdentity>().title = std::move(title); }

void Actor::setPosition(s32 const x, s32 const y, s32 const z)
{
    auto & pos = component<Position>();
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void Actor::setPosX(s32 const x) { component<Position>().x = x; }
void Actor::setPosY(s32 const y) { component<Position>().y = y; }
void Actor::setPosZ(s32 const z) { component<Position>().z = z; }

void Actor::setTeam(Team const team) { _impl->team = team; }
