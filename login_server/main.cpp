#include <boost/asio.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

int main()
{
#ifdef NDEBUG
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#)");
#else
    spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v (%s:%#:%!)");
#endif

    boost::asio::io_context io;
    boost::asio::post(io, [] { SPDLOG_INFO("Hello, World!"); });
    io.run();

    return EXIT_SUCCESS;
}
