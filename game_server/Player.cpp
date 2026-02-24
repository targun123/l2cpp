/// @author    Chnossos
/// @date      Created on 2026-02-24

#include "Player.hpp"

// Project includes
#include "game/Character.hpp"
#include "network/Connection.hpp"

// C++ includes
#include <vector>

struct Player::PlayerImpl
{
    Network::Connection    conn;
    std::wstring           accountName;
    u32                    playOk1;
    std::vector<Character> characters;

    explicit PlayerImpl(boost::asio::ip::tcp::socket && socket);
};

Player::PlayerImpl::PlayerImpl(boost::asio::ip::tcp::socket && socket)
    : conn(std::move(socket))
{
    characters.emplace_back();
}

Player::Player(boost::asio::ip::tcp::socket && socket)
    : _impl(std::make_unique<Player::PlayerImpl>(std::move(socket)))
{}

Player::~Player() = default;

auto Player::connection() -> Network::Connection &
{
    return _impl->conn;
}

auto Player::connection() const -> Network::Connection const &
{
    return _impl->conn;
}

auto Player::accountName() const -> std::wstring_view
{
    return _impl->accountName;
}

auto Player::playOk1() const -> u32
{
    return _impl->playOk1;
}

auto Player::characters() -> std::span<Character>
{
    return _impl->characters;
}

auto Player::characters() const -> std::span<Character const>
{
    return _impl->characters;
}

void Player::setAccountName(std::wstring userName)
{
    _impl->accountName = std::move(userName);
}

void Player::setPlayOk1(u32 playOk1)
{
    _impl->playOk1 = playOk1;
}

auto Player::addCharacter() -> Character &
{
    return _impl->characters.emplace_back();
}
