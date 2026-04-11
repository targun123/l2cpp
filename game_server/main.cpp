/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "Application.hpp"

#include <l2cpp/CompileTimeConfig.hpp>
#include <l2cpp/Exception.hpp>

// Third-party
#include <spdlog/spdlog.h>

int main(int const argc, char const * const argv[]) try
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if constexpr (Config::isDebugMode)
        spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
    else
        spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");

    spdlog::set_level(spdlog::level::trace);

    Application app({argv, argv + argc});
    return app.load() && app.run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (l2cpp::Exception const & e)
{
    SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    return EXIT_FAILURE;
}
#ifdef NDEBUG
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
#endif
