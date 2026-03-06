/// @author    Chnossos
/// @date      Created on 2026-03-03

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

namespace l2 { class GameObject; }

class l2::GameObject
{
public:
    GameObject();
    GameObject(GameObject &&) noexcept;
    GameObject & operator=(GameObject &&) noexcept;
    virtual ~GameObject() = 0;

public:
    auto id() const -> GameObjectId;

private:
    struct GameObjectImpl;
    Pimpl<GameObjectImpl> _impl;
};
