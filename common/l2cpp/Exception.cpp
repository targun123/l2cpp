/// @author    Chnossos
/// @date      Created on 20/07/2021

#include "Exception.hpp"

namespace
{
    auto fileName(std::string_view path) -> std::string_view
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
        constexpr char separator = '\\';
#else
        constexpr char separator = '/';
#endif

        auto const pos = path.find_last_of(separator);
        return pos != std::string::npos ? path.substr(pos + 1) : path;
    }

    void getExceptionStack(l2cpp::Exception const & e, l2cpp::ExceptionStack & stack)
    {
        stack.emplace_back(e);
        try
        {
            std::rethrow_if_nested(e);
        }
        catch (l2cpp::Exception const & nested)
        {
            getExceptionStack(nested, stack);
        }
        catch (std::exception const & nested)
        {
            getExceptionStack(l2cpp::Exception({}, 0, "{}",  nested.what()), stack);
        }
    }

    std::string formatSourceLocation(l2cpp::Exception const & e)
    {
        std::string src;

        if (!e.fileBaseName().empty())
            src += e.fileBaseName();

        if (e.line() > 0)
        {
            if (!src.empty())
                src += ':';

            src += fmt::format("{}", e.line());
        }

        if (!src.empty())
            src = fmt::format(" [{}]", src);

#ifndef NDEBUG
        if (!e.functionName().empty())
            src += fmt::format(" [{}]", e.functionName());
#endif
        return src;
    }
} // !namespace

l2cpp::Exception::Exception() noexcept
    : Exception(0)
{}

l2cpp::Exception::Exception(int const code) noexcept
    : Exception({}, code)
{}

l2cpp::Exception::Exception(std::source_location src, int const code) noexcept
    : _source(std::move(src))
    , _code(code)
{}

auto l2cpp::Exception::code() const -> int { return _code;          }
auto l2cpp::Exception::line() const -> int { return _source.line(); }

auto l2cpp::Exception::fileBaseName() const  -> std::string_view
{
    return fileName(_source.file_name());
}

auto l2cpp::Exception::functionName() const  -> std::string_view { return _source.function_name(); }
auto l2cpp::Exception::what() const noexcept -> char const *     { return _message.c_str();        }

void l2cpp::Exception::init(std::source_location src, std::string message)
{
    init(std::move(src), 0, std::move(message));
}

void l2cpp::Exception::init(std::source_location src, int const code)
{
    init(std::move(src), code, std::string());
}

void l2cpp::Exception::init(std::source_location src, int const code, std::string message)
{
    _source  = std::move(src);
    _message = std::move(message);
    _code    = code;
}

auto l2cpp::sandBox(std::source_location src, std::function<void()> f) -> ExceptionBox
{
    ExceptionBox box { nullptr, std::move(src) };
    try         { f();                                }
    catch (...) { box.ptr = std::current_exception(); }
    return box;
}

auto l2cpp::getExceptionStack(Exception const & e) -> ExceptionStack
{
    ExceptionStack stack;
    ::getExceptionStack(e, stack);
    return stack;
}

auto l2cpp::formatExceptionStack(std::exception const & e, int level, int increment,
                                 std::string_view prefix, std::string_view suffix)
    -> std::string
{
    ExceptionStack stack;
    stack.emplace_back(Exception("{}",  e.what()));
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (Exception const & nested)
    {
        ::getExceptionStack(nested, stack);
    }
    catch (std::exception const & nested)
    {
        ::getExceptionStack(Exception("{}",  nested.what()), stack);
    }
    return formatExceptionStack(stack, level, increment, prefix, suffix);
}

auto l2cpp::formatExceptionStack(Exception const & e, int level, int increment,
                                 std::string_view prefix, std::string_view suffix)
    -> std::string
{
    return formatExceptionStack(getExceptionStack(e), level, increment, prefix, suffix);
}

auto l2cpp::formatExceptionStack(ExceptionStack const & s, int level, int increment,
                                 std::string_view prefix, std::string_view suffix)
    -> std::string
{
    std::string str;
    for (auto const & e : s)
    {
        auto const code = e.code() ? fmt::format(" (code: {})", e.code()) : "";
        auto const src  = formatSourceLocation(e);
        str += fmt::format("{:>{}}{}{}{}{}",
                           prefix, static_cast<int>(prefix.size()) + level, e.what(), code, src, suffix);
        level += increment;
    }
    return str;
}
