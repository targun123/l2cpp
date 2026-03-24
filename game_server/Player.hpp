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

class Player
{
public:
    explicit Player(boost::asio::ip::tcp::socket && socket);
    ~Player();

public:
    auto connection()       -> Network::Connection &;
    auto connection() const -> Network::Connection const &;

    auto characters()       -> std::span<Ref<Character>>;
    auto characters() const -> std::span<Ref<Character> const>;
    auto currentCharacter() -> OptRef<Character>;

    auto accountName()   const -> std::wstring_view;
    auto playOk1()       const -> u32;

public:
    void setAccountName(std::wstring userName);
    void setPlayOk1(u32 playOk1);
    auto addCharacter() -> Character &;
    void setCurrentCharacter(size_t index);

private:
    struct PlayerImpl;
    Pimpl<PlayerImpl> _impl;
};
