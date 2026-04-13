/// @author    Chnossos
/// @date      Created on 2026-02-17

// Project includes
#include "Application.hpp"

#include <l2cpp/CompileTimeConfig.hpp>
#include <l2cpp/Exception.hpp>

// Third-party
#include <spdlog/spdlog.h>

static void setupLogger();

int main(int const argc, char const * const argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    setupLogger();

    int code = EXIT_FAILURE;
    try
    {
        Application app({argv, argv + argc});
        if (app.load())
            code = app.run();
    }
    catch (l2cpp::Exception const & e)
    {
        SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    }
    catch (std::exception const & e)
    {
        SPDLOG_CRITICAL("Caught top-level exception:\n{}", l2cpp::formatExceptionStack(e));
    }
    catch (...)
    {
        SPDLOG_CRITICAL("Unexpected top-level exception caught, terminating");
    }
    return code;
}

void setupLogger()
{
    if constexpr (Config::isDebugMode)
        spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#] [%!()]");
    else
        spdlog::set_pattern("[%Y-%m-%d %R:%S.%e] [%^%L%$] %v [%s:%#]");

    spdlog::set_level(spdlog::level::trace);
}
