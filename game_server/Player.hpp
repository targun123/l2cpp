/// @author    Chnossos
/// @date      Created on 2026-02-24

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

// Third-party includes
#include <boost/asio/ip/tcp.hpp>

// C++ includes
#include <deque>
#include <span>

class Character;
namespace Network { class Connection; }

class Action
{
public:
    enum class Type
    {
        Move
    };

public:
    explicit Action(Type type) noexcept: _type(type) {}
    virtual ~Action() noexcept = default;

public:
    Type type() const { return _type; }

private:
    Type _type;
};

struct MoveAction : public Action
{
    MoveAction() noexcept
        : Action(Type::Move)
        , startTime(std::chrono::steady_clock::now())
        , lastUpdateTime(startTime)
    {}

    s32 originX = 0, originY = 0, originZ = 0;
    s32 targetX = 0, targetY = 0, targetZ = 0;

    float currentDistance = 0, totalDistance = 0;

    std::chrono::steady_clock::time_point startTime, lastUpdateTime;

    enum class Input { Keyboard, Mouse } input;
};

class Player
{
public:
    explicit Player(boost::asio::ip::tcp::socket && socket);
    ~Player();

public:
    auto connection()       -> Network::Connection &;
    auto connection() const -> Network::Connection const &;

    auto characters()       -> std::span<Character>;
    auto characters() const -> std::span<Character const>;
    auto currentCharacter() -> OptionalRef<Character>;

    auto actions() -> std::deque<std::unique_ptr<Action>> &;

    auto accountName()   const -> std::wstring_view;
    auto playOk1()       const -> u32;

public:
    void setAccountName(std::wstring userName);
    void setPlayOk1(u32 playOk1);
    auto addCharacter() -> Character &;
    void setCurrentCharacter(size_t index);

    template<typename A, typename... Args, typename = std::enable_if_t<std::is_base_of_v<Action, A>>>
    auto setNextAction(Args &&... args)
    {
        actions().clear();
        return static_cast<A &>(*actions().emplace_back(std::make_unique<A>(std::forward<Args>(args)...)));
    }

private:
    struct PlayerImpl;
    Pimpl<PlayerImpl> _impl;
};
