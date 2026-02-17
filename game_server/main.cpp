/// @author    Chnossos
/// @date      Created on 2026-02-17

#include <l2cpp/Exception.hpp>
#include <l2cpp/network/SocketListener.hpp>

#include <spdlog/spdlog.h>

int main() try
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#)");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#:%!)");
#endif
    spdlog::set_level(spdlog::level::trace);

    constexpr auto ip   = "127.0.0.1";
    constexpr auto port = 7777;

    boost::asio::io_context ioContext;
    l2cpp::Network::SocketListener socketListener(ioContext);

    auto onSocketAccepted = [] (boost::asio::ip::tcp::socket &&) {};
    L2CPP_B_ASSERT(socketListener.listen(ip, port, std::move(onSocketAccepted)),
                   "Failed to listen on {}:{}", ip, port);

    ioContext.run();
    return EXIT_SUCCESS;
}
catch (l2cpp::Exception const & e)
{
    l2cpp::printExceptionStack(e);
    return EXIT_FAILURE;
}
catch (std::exception const & e)
{
    l2cpp::printExceptionStack(e);
    return EXIT_FAILURE;
}
catch (...)
{
    SPDLOG_CRITICAL("Unexpected exception caught, terminating");
    return EXIT_FAILURE;
}
