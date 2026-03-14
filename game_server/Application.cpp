/// @author    Chnossos
/// @date      Created on 2026-03-06

#include "Application.hpp"

// Project includes
#include "Player.hpp"
#include "game/skill/SkillInfoRegister.hpp"
#include "handlers/PacketHandlers.hpp"
#include "network/Connection.hpp"
#include "network/packets/server/ClientDisconnect.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/details/Pimpl.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party includes
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

// C++ includes
#include <iostream>

struct Application::ApplicationImpl
{
    std::vector<std::string_view>  args;
    boost::asio::io_context        ioContext;
    boost::asio::signal_set        signalSet;
    l2cpp::Network::SocketListener socketListener;
    std::list<Player>              players;

    explicit ApplicationImpl(std::vector<std::string_view> args_)
        : args(std::move(args_))
        , signalSet(ioContext, SIGINT)
        , socketListener(ioContext)
    {}

    bool load();
    bool run();
    void shutdown();

private:
    void onSignal(boost::system::error_code const & ec, int signal);
    void onSocketAccepted(boost::asio::ip::tcp::socket socket);
};

template class Pimpl<Application::ApplicationImpl>;

bool Application::ApplicationImpl::load() try
{
    SkillInfoRegister::load("data/skillname-e.txt", "data/skillgrp.txt");
    SPDLOG_INFO("Registered {} unique skill variants", SkillInfoRegister::size());

    return true;
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_ERROR("Failed to load application:\n{}", l2cpp::formatExceptionStack(e));
    return false;
}

bool Application::ApplicationImpl::run()
{
    signalSet.async_wait([this] (auto const & ec, int s) { onSignal(ec, s); });

    constexpr auto ip   = "127.0.0.1";
    constexpr auto port = 7777;

    auto socketAcceptedCb = [this] (auto socket) { onSocketAccepted(std::move(socket)); };
    L2CPP_B_ASSERT(socketListener.listen(ip, port, socketAcceptedCb), "Failed to listen on {}:{}", ip, port);
    SPDLOG_INFO("Listening for clients on {}:{}", ip, port);

    SPDLOG_INFO("Server running. Input CTRL+C to initiate shutdown…");
    ioContext.run();

    SPDLOG_INFO("Goodbye.");
    return EXIT_SUCCESS;
}

void Application::ApplicationImpl::shutdown()
{
    SPDLOG_INFO("Beginning shutting down sequence…");

    SPDLOG_INFO("Shutting down SocketListener…");
    socketListener.shutdown();

    SPDLOG_INFO("Kicking players out…");
    for (auto & player : players)
    {
        if (player.connection().isAlive())
        {
            // This packet displays the disconnected message box, but doesn't close the connection until OK is clicked
            player.connection().send(Network::Packet::Server::ClientDisconnect());
            player.connection().close();
        }
    }

    boost::asio::post(ioContext, [] { SPDLOG_INFO("Shutting down sequence done."); });
}

void Application::ApplicationImpl::onSignal(boost::system::error_code const & ec, int)
{
    switch (ec.default_error_condition().value())
    {
        case boost::system::errc::success:
            SPDLOG_INFO("SIGINT received");
            shutdown();
            break;

        default:
            SPDLOG_WARN("onSignal error {}: {}", ec.default_error_condition().value(), ec.message());
            break;
    }
}

void Application::ApplicationImpl::onSocketAccepted(boost::asio::ip::tcp::socket socket) try
{
    static auto hexdump = [] (std::span<byte const> const buffer)
    {
#ifndef NDEBUG
        if (!buffer.empty())
            std::cout << l2cpp::hexdump(buffer.data(), buffer.size()) << std::endl;
#endif
    };

    auto & player = players.emplace_back(std::move(socket));
    auto onPacketReceived = [&player] (std::span<byte const> const buffer)
    {
        auto const size   = *reinterpret_cast<PacketHeader const *>(buffer.data());
        auto const opCode = buffer[sizeof(size)]; // client opcode is always one byte
        auto const body   = buffer.subspan(sizeof(size) + sizeof(opCode));

        if (auto const it = gPacketHandlers.find(opCode); it != gPacketHandlers.end())
        {
            auto const & [handler, handlerName] = it->second;
            SPDLOG_INFO("recv: {} (0x{:02x}) ({} bytes)", handlerName, opCode, size);
            hexdump(body);
            (*handler)(player);
        }
        else
        {
            SPDLOG_WARN("Unsupported packet 0x{:02x} ({} bytes)", opCode, size);
            hexdump(body);
        }

        if (player.connection().isAlive())
            player.connection().asyncReadNextPacket();
    };

    player.connection().setOnPacketReceivedHandler([this, onPacketReceived] (std::span<byte const> buffer) {
        boost::asio::post(ioContext, std::bind(onPacketReceived, buffer));
    });
    player.connection().asyncReadNextPacket();
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_ERROR("Packet reading failed, disconnecting client:\n{}", l2cpp::formatExceptionStack(e));
}

// ---------------------------------------------------------------------------------------------------------------------

Application::Application(std::vector<std::string_view> args): _impl(std::move(args)) {}
Application::~Application() = default;

bool Application::load() { return _impl->load(); }
bool Application::run()  { return _impl->run();  }
