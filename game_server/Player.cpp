/// @author    Chnossos
/// @date      Created on 2026-02-24

#include "Player.hpp"

// Project includes
#include "game/World.hpp"
#include "game/actor/Character.hpp"
#include "network/Connection.hpp"

#include <l2cpp/Exception.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

struct Player::PlayerImpl
{
    Network::Connection conn;
    std::wstring        accountName;
    u32                 playOk1;

    OptRef<Character> currentCharacter;

    explicit PlayerImpl(boost::asio::ip::tcp::socket && socket);
};

template class Pimpl<Player::PlayerImpl>;

Player::PlayerImpl::PlayerImpl(boost::asio::ip::tcp::socket && socket)
    : conn(std::move(socket))
    , playOk1()
{}

// ---------------------------------------------------------------------------------------------------------------------

Player::Player(boost::asio::ip::tcp::socket && socket)
    : _impl(std::move(socket))
{}

Player::~Player() = default;

auto Player::connection()       -> Network::Connection       & { return _impl->conn; }
auto Player::connection() const -> Network::Connection const & { return _impl->conn; }

auto Player::currentCharacter() -> OptRef<Character> { return _impl->currentCharacter;  }

auto Player::accountName() const -> std::wstring_view { return _impl->accountName; }
auto Player::playOk1()     const -> u32               { return _impl->playOk1;     }

void Player::setAccountName(std::wstring userName) { _impl->accountName = std::move(userName); }
void Player::setPlayOk1(u32 const playOk1)         { _impl->playOk1 = playOk1;                 }

void Player::setCurrentCharacter(Character & c)
{
    _impl->currentCharacter = c;
    c.player = *this;
}

void Player::unsetCurrentCharacter()
{
    _impl->currentCharacter.reset();
}
