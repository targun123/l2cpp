/// @author    Chnossos
/// @date      Created on 2026-02-13

// Project includes
#include "Packets.hpp"
#include "crypto/Blowfish.hpp"
#include "crypto/Checksum.hpp"
#include "crypto/Rsa.hpp"

#include <l2cpp/Exception.hpp>
#include <l2cpp/Typedefs.hpp>
#include <l2cpp/network/Packet.hpp>
#include <l2cpp/network/SocketListener.hpp>

// Third-party includes
#include <boost/asio.hpp>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <spdlog/spdlog.h>

using boost::asio::ip::tcp;
using l2cpp::Network::Packet;

struct Connection
{
    u32             sessionId;
    tcp::socket     socket;

    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;
    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;

    std::vector<byte> readBuffer;

    std::string userName;
    std::string password;

    std::function<void()>                          onSocketClosed;
    std::function<void(std::vector<byte> const &)> onPacketRead;

    explicit Connection(tcp::socket socket)
        : socket(std::move(socket))
        , bigNum(nullptr, &BN_free)
        , rsaKey(RSA_new(), &RSA_free)
    {
        static decltype(sessionId) nextSessionId = 0;
        sessionId = nextSessionId++;

        BIGNUM * n = nullptr;
        BN_dec2bn(&n, "65537");
        bigNum.reset(n);

        RSA_generate_key_ex(rsaKey.get(), 1024, bigNum.get(), nullptr);
        readBuffer.resize(sizeof(u16) + sizeof(u8) + RSA_size(rsaKey.get())); // size + opCode + rsa
    }

    bool operator==(Connection const & other) const { return sessionId == other.sessionId; }

    void close()
    {
        if (socket.is_open())
        {
            boost::system::error_code ec;
            if (socket.shutdown(socket.shutdown_both, ec))
                SPDLOG_ERROR("shutdown error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

            if (socket.close(ec))
                SPDLOG_ERROR("close error '{}' ({}): {}", ec.category().name(), ec.value(), ec.message());

            if (onSocketClosed)
                onSocketClosed();
        }
    }

    void asyncReadNextPacket()
    {
        L2CPP_B_ASSERT(socket.is_open(), "Cannot read another packet: socket not open");
        boost::asio::async_read(socket, boost::asio::buffer(readBuffer, sizeof(PacketHeader)),
                                [this] (auto const & ec, auto) { onSizeRead(ec); });
    }

    void send(Packet & p, bool const encryptPacket = true)
    {
        static std::array<byte, 7> pad{};

        // Align to 8 bytes then append checksum
        if (auto const overflow = p.bodySize() % 8)
            p << std::span(pad.data(), 8 - overflow);

        p << Checksum::calculate({p.body().data(), p.bodySize()});
        p.finalize();

        if (encryptPacket)
            Blowfish::encrypt(p.body());

        boost::asio::write(socket, boost::asio::buffer(p.buffer()));
        SPDLOG_INFO("'{}' ← 0x{:02x} ({:>3} bytes)", sessionId, p.opCode(), p.size());
    }

private:
    void onSizeRead(boost::system::error_code const & ec)
    {
        if (!handleError(ec))
            return close();

        readBuffer.resize(*reinterpret_cast<PacketHeader const *>(readBuffer.data()));

        auto const payload = std::span(readBuffer).subspan(sizeof(PacketHeader));
        boost::asio::async_read(socket, boost::asio::buffer(payload), [this] (auto const & e, auto) { onBodyRead(e); });
    }

    void onBodyRead(boost::system::error_code const & ec)
    {
        if (!handleError(ec))
            return close();

        if (onPacketRead)
            onPacketRead(readBuffer);
    }

    /// @returns @c true if the async operation can be executed again, else @c false
    bool handleError(boost::system::error_code const & ec) const
    {
        switch (auto const code = ec.value())
        {
            case boost::system::errc::success:
                return true;

            case boost::asio::error::eof:
                SPDLOG_INFO("Client {} disconnected by themselves", sessionId);
                break;

            case boost::asio::error::operation_aborted:
                SPDLOG_INFO("Client {} disconnected by server", sessionId);
                break;

            default:
                SPDLOG_ERROR("Something went wrong during packet body reading (code {}): {}", code, ec.message());
                break;
        }
        return false;
    }
};

static void sendInitPacket(Connection & conn)
{
    constexpr u32 protocol = 0xc621;

    Packet p(SentPacket::Initialization);
    p << conn.sessionId << protocol << Rsa::instance().modulus();
    conn.send(p, false);

    conn.asyncReadNextPacket();
}

static void handleGameGuardPacket(Connection & conn)
{
    constexpr u32 ignoreGg = 0x0b;
    conn.send(Packet(SentPacket::GameGuard) << ignoreGg);
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
        return conn.send(Packet(SentPacket::AuthenticationFailed) << 0x01); // system error
    }

    conn.userName = reinterpret_cast<char const *>(body.data() + 0x62);
    conn.password = reinterpret_cast<char const *>(body.data() + 0x70);

    std::array<byte, sizeof(u64)> loginKey;
    RAND_bytes(loginKey.data(), static_cast<int>(loginKey.size()));
    conn.send(Packet(SentPacket::AuthenticationSuccess) << loginKey);
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

    Packet p(SentPacket::GameServerList);
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
    u32 playOk1, playOk2;
    RAND_bytes(reinterpret_cast<byte *>(&playOk1), sizeof(playOk1));
    RAND_bytes(reinterpret_cast<byte *>(&playOk2), sizeof(playOk2));

    Packet p(SentPacket::GameServerSelectionSuccess);
    p << playOk1 << playOk2;
    conn.send(p);
}

static void handlePacket(Connection & conn)
{
    auto const payload = std::span(conn.readBuffer).subspan(sizeof(PacketHeader));
    Blowfish::decrypt(payload);
    L2CPP_B_ASSERT(Checksum::verify(payload), "Checksum verification failed: checksums differ");

    auto const opCode = payload[0];
    SPDLOG_INFO("'{}' → 0x{:02x} ({:>3} bytes)", conn.sessionId, opCode, conn.readBuffer.size());

    static std::unordered_map<RecvPacket, void(*)(Connection &)> const handlers
    {
        { RecvPacket::Authentication,      &handleAuthPacket            },
        { RecvPacket::GameServerSelection, &handleServerSelectionPacket },
        { RecvPacket::GameServerList,      &handleServerListPacket      },
        { RecvPacket::GameGuard,           &handleGameGuardPacket       },
    };

    if (auto const it = handlers.find(static_cast<RecvPacket>(opCode)); it != handlers.end()) try
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
        for (auto & conn : connections)
        {
            conn.onSocketClosed = nullptr;
            conn.close();
        }

        listener.shutdown();
    });

    auto onSocketAccepted = [&] (tcp::socket && socket)
    {
        SPDLOG_INFO("Client connected!");
        auto & conn = connections.emplace_back(std::move(socket));
        conn.onSocketClosed = [&] { connections.remove(conn);          };
        conn.onPacketRead   = [&] (auto const &) { handlePacket(conn); };
        sendInitPacket(conn);
    };
    if (!listener.listen("127.0.0.1", 2106, std::move(onSocketAccepted)))
        return EXIT_FAILURE;

    SPDLOG_INFO("Listening for clients on 127.0.0.1:2106");
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
