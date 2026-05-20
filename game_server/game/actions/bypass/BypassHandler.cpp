#include "BypassHandler.hpp"
#include "impl/ChatLink.hpp"
#include <algorithm>
#include "impl/QuestLink.hpp"


static auto toLowerString(std::string_view view) -> std::string
{
    std::string str(view);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
        });
    return str;
}

void BypassHandler::registerHandler(std::shared_ptr<IBypassHandler> const& handler)
{
    
    std::unique_lock lock(_mutex);
    for (auto const& cmd : handler->CommandList())
    {
        _datatable[toLowerString(cmd)] = handler;
    }
}

void BypassHandler::removeHandler(std::shared_ptr<IBypassHandler> const& handler)
{
    
    std::unique_lock lock(_mutex);
    for (auto const& cmd : handler->CommandList())
    {
        _datatable.erase(toLowerString(cmd));
    }
}

auto BypassHandler::Handler(std::string_view commandValue) -> std::shared_ptr<IBypassHandler>
{
    
    if (size_t const spacePos = commandValue.find(' '); spacePos != std::string_view::npos)
    {
        commandValue = commandValue.substr(0, spacePos);
    }

    
    std::shared_lock lock(_mutex);

    
    std::string const lowerCmd = toLowerString(commandValue);

    if (auto const it = _datatable.find(lowerCmd); it != _datatable.end())
    {
        return it->second;
    }

    return nullptr;
}

auto BypassHandler::size() const -> size_t
{
    std::shared_lock lock(_mutex);
    return _datatable.size();
}

void BypassHandler::load()
{

    auto& manager = BypassHandler::instance();

    manager.registerHandler(std::make_shared<ChatLink>());
    manager.registerHandler(std::make_shared<QuestLink>());

   
}