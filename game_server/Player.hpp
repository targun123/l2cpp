/// @author    Chnossos
/// @date      Created on 2026-02-24

#pragma once

// Project includes
#include <l2cpp/Pimpl.hpp>
#include <l2cpp/Typedefs.hpp>

// Third-party includes
#include <boost/asio/ip/tcp.hpp>

// C++ includes
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

    auto characters()       -> std::span<Character>;
    auto characters() const -> std::span<Character const>;

    auto accountName()   const -> std::wstring_view;
    auto playOk1()       const -> u32;

public:
    void setAccountName(std::wstring userName);
    void setPlayOk1(u32 playOk1);
    auto addCharacter() -> Character &;

private:
    struct PlayerImpl;
    Pimpl<PlayerImpl> _impl;
};
