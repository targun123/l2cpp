/// @author    Chnossos
/// @date      Created on 2026-02-24

#include "Player.hpp"

// Project includes
#include "game/World.hpp"
#include "game/actor/Character.hpp"
#include "network/Connection.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/details/Pimpl.hpp>

// C++ includes
#include <deque>
#include <vector>

struct Player::PlayerImpl
{
    Network::Connection conn;
    std::wstring        accountName;
    u32                 playOk1;

    std::vector<Ref<Character>> characters;
    OptRef<Character>           currentCharacter;

    explicit PlayerImpl(boost::asio::ip::tcp::socket && socket);
};

Player::PlayerImpl::PlayerImpl(boost::asio::ip::tcp::socket && socket)
    : conn(std::move(socket))
    , playOk1()
{}

template class Pimpl<Player::PlayerImpl>;

// ---------------------------------------------------------------------------------------------------------------------

Player::Player(boost::asio::ip::tcp::socket && socket)
    : _impl(std::move(socket))
{
    auto & c = _impl->characters.emplace_back(World::addCharacter(*this)).get();
    c.setName(L"test" + std::to_wstring(c.id()));
    c.setTitle(L"{l2cpp}");
}

Player::~Player() = default;

auto Player::connection()       -> Network::Connection       & { return _impl->conn; }
auto Player::connection() const -> Network::Connection const & { return _impl->conn; }

auto Player::characters()       -> std::span<Ref<Character>>       { return _impl->characters;       }
auto Player::characters() const -> std::span<Ref<Character> const> { return _impl->characters;       }
auto Player::currentCharacter() -> OptRef<Character>               { return _impl->currentCharacter; }

auto Player::accountName() const -> std::wstring_view { return _impl->accountName; }
auto Player::playOk1()     const -> u32               { return _impl->playOk1;     }

void Player::setAccountName(std::wstring userName) { _impl->accountName = std::move(userName); }
void Player::setPlayOk1(u32 const playOk1)         { _impl->playOk1 = playOk1;                 }

auto Player::addCharacter() -> Character &
{
    return _impl->characters.emplace_back(World::addCharacter(*this)).get();
}

void Player::setCurrentCharacter(size_t const index)
{
    L2CPP_B_ASSERT(index < _impl->characters.size(), "Attempt to use non existent character (index: {})", index);
    _impl->currentCharacter = _impl->characters[index];
}
