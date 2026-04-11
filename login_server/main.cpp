/// @author    Chnossos
/// @date      Created on 2026-02-13

// Project includes
#include "Constants.hpp"
#include "crypto/Blowfish.hpp"
#include "crypto/Checksum.hpp"
#include "crypto/Rsa.hpp"
#include "network/Connection.hpp"
#include "network/OpCodes.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party includes
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <openssl/rand.h>
#include <spdlog/spdlog.h>

using l2cpp::Network::Packet;

static void sendInitPacket(Connection & conn)
{
    Packet p(ServerOpCode::Initialization);
    p << conn.sessionId << Constants::protocol << Rsa::instance().modulus();
    conn.send(p, false);
}

static void handleGameGuardPacket(Connection & conn)
{
    constexpr u32 ignoreGg = 0x0b;
    conn.send(Packet(ServerOpCode::GameGuard) << ignoreGg);
}

static void handleAuthPacket(Connection & conn)
{
    auto const body = std::span(conn.readBuffer).subspan(sizeof(PacketHeader) + sizeof(PacketOpCode));

    try
    {
        Rsa::instance().decrypt(body);
    }
    catch (l2cpp::Exception const & e)
    {
        SPDLOG_ERROR("Rsa::decrypt failed:\n{}", l2cpp::formatExceptionStack(e));
        return conn.send(Packet(ServerOpCode::AuthenticationFailed) << 0x01); // system error
    }

    conn.userName = reinterpret_cast<char const *>(body.data() + 0x62);
    conn.password = reinterpret_cast<char const *>(body.data() + 0x70);

    std::array<byte, sizeof(u64)> loginKey;
    RAND_bytes(loginKey.data(), static_cast<int>(loginKey.size()));
    conn.send(Packet(ServerOpCode::AuthenticationSuccess) << loginKey);
}

static void handleServerListPacket(Connection & conn)
{
    struct ServerInfo
    {
        u8   id             = 1;
        u8   host[4]        = {127, 0, 0, 1};
        u32  port           = 7777;
        u8   ageLimit       = 0;
        bool isPvp          = true;
        u16  curPlayerCount = 0;
        u16  maxPlayerCount = 1'000;
        bool isOnline       = true;
        u32  extra          = 0;
        u8   brackets       = 0;
    } const onlineServer, offlineServer{.id = 2, .isPvp = false, .isOnline = false};

    constexpr u8 serverCount = 2;

    Packet p(ServerOpCode::GameServerList);
    p << serverCount << 0_u8; // ?

    for (auto const srv : {&onlineServer, &offlineServer})
    {
        p
            << srv->id
            << srv->host
            << srv->port
            << srv->ageLimit
            << srv->isPvp
            << srv->curPlayerCount
            << srv->maxPlayerCount
            << srv->isOnline
            << srv->extra
            << srv->brackets
        ;
    }

    conn.send(p);
}

static void handleServerSelectionPacket(Connection & conn)
{
    std::array<byte, sizeof(u64)> gameKey;
    RAND_bytes(gameKey.data(), static_cast<int>(gameKey.size()));
    conn.send(Packet(ServerOpCode::GameServerSelectionSuccess) << gameKey);
}

static void handlePacket(Connection & conn)
{
    auto const payload = std::span(conn.readBuffer).subspan(sizeof(PacketHeader));
    Blowfish::decrypt(payload);
    L2CPP_B_ASSERT(Checksum::verify(payload), "Checksum verification failed: checksums differ");

    auto const opCode = payload[0];
    SPDLOG_INFO("'{}' → 0x{:02x} ({:>3} bytes)", conn.sessionId, opCode, conn.readBuffer.size());

    static std::unordered_map<ClientOpCode, void(*)(Connection &)> const handlers
    {
        { ClientOpCode::Authentication,      &handleAuthPacket            },
        { ClientOpCode::GameServerSelection, &handleServerSelectionPacket },
        { ClientOpCode::GameServerList,      &handleServerListPacket      },
        { ClientOpCode::GameGuard,           &handleGameGuardPacket       },
    };

    if (auto const it = handlers.find(static_cast<ClientOpCode>(opCode)); it != handlers.end()) try
    {
        (*it->second)(conn);
    }
    catch (l2cpp::Exception const & e)
    {
        SPDLOG_ERROR("Something wrong happened during packet handling of connection {}. Closing connection."
                     "\nBacktrace:\n{}", conn.sessionId, l2cpp::formatExceptionStack(e));
        return conn.close();
    }
    else
        SPDLOG_WARN("Received unknown packet type '{}'", opCode);

    conn.asyncReadNextPacket();
}

int main() try
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
#endif
    spdlog::set_level(spdlog::level::trace);

    std::list<Connection> connections;

    boost::asio::io_context ioContext;
    l2cpp::Network::SocketListener listener(ioContext);

    boost::asio::signal_set signals(ioContext, SIGINT);
    signals.async_wait([&] (auto const &, auto)
    {
        SPDLOG_INFO("Shutdown sequence requested");

        SPDLOG_INFO("Kicking players out");
        for (auto & conn : connections)
        {
            conn.onSocketClosed = nullptr;
            conn.close();
        }

        listener.shutdown();
    });

    auto onSocketAccepted = [&] (boost::asio::ip::tcp::socket && socket)
    {
        SPDLOG_INFO("Client connected!");
        auto & conn = connections.emplace_back(std::move(socket));
        conn.onSocketClosed = [&] { connections.remove(conn);          };
        conn.onPacketRead   = [&] (auto const &) { handlePacket(conn); };

        sendInitPacket(conn);
        conn.asyncReadNextPacket();
    };
    if (!listener.listen(Constants::host, Constants::port, std::move(onSocketAccepted)))
        return EXIT_FAILURE;

    SPDLOG_INFO("Listening for clients on {}:{}", Constants::host, Constants::port);
    ioContext.run();

    SPDLOG_INFO("Goodbye!");
    return EXIT_SUCCESS;
}
catch (std::exception const & e)
{
    SPDLOG_CRITICAL("Exception caught: '{}'", e.what());
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unknown exception caught");
    return EXIT_FAILURE;
}
