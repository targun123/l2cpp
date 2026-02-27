/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "Player.hpp"
#include "handlers/PacketHandlers.hpp"
#include "network/Connection.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Misc.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party
#include <spdlog/spdlog.h>

// C++ includes
#include <iostream>

static void hexdump(std::span<byte const> const buffer)
{
#ifndef NDEBUG
    if (!buffer.empty())
        std::cout << l2cpp::hexdump(buffer.data(), buffer.size()) << std::endl;
#endif
}

static void onSocketAccepted(boost::asio::ip::tcp::socket && socket) try
{
    Player player(std::move(socket));

    while (player.connection().isAlive())
    {
        player.connection().read();

        auto const buffer = player.connection().readBuffer();
        auto const opCode = buffer[2];
        auto const size   = *reinterpret_cast<u16 const *>(buffer.data());

        if (auto const it = gPacketHandlers.find(opCode); it != gPacketHandlers.end())
        {
            auto const & [handler, handlerName] = it->second;
            SPDLOG_INFO("recv: {} (0x{:02x}) ({} bytes)", handlerName, opCode, size);
            ::hexdump(buffer.subspan(3, size - 3));
            (*handler)(player);
        }
        else
        {
            SPDLOG_WARN("Unsupported packet 0x{:02x} ({} bytes)", opCode, size);
            ::hexdump(buffer.subspan(3, size - 3));
        }
    }
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_ERROR("Packet reading failed, disconnecting client:\n{}", l2cpp::formatExceptionStack(e));
}

int main() try
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
#endif
    spdlog::set_level(spdlog::level::trace);

    constexpr auto ip   = "127.0.0.1";
    constexpr auto port = 7777;

    boost::asio::io_context        ioContext;
    l2cpp::Network::SocketListener socketListener(ioContext);
    L2CPP_B_ASSERT(socketListener.listen(ip, port, &onSocketAccepted), "Failed to listen on {}:{}", ip, port);

    ioContext.run();
    return EXIT_SUCCESS;
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    return EXIT_FAILURE;
}
catch (std::exception const & e)
{
    SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unexpected top-level exception caught, terminating");
    return EXIT_FAILURE;
}
