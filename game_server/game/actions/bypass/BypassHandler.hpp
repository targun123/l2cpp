#pragma once
#pragma once
#include "IBypassHandler.hpp"
#include <unordered_map>
#include <string>
#include <shared_mutex>

class BypassHandler
{
private:
    
    BypassHandler() = default;

    
    std::unordered_map<std::string, std::shared_ptr<IBypassHandler>> _datatable;

    
    mutable std::shared_mutex _mutex;

public:
    
    ~BypassHandler() = default;
    BypassHandler(BypassHandler const&) = delete;
    BypassHandler& operator=(BypassHandler const&) = delete;

   
    static auto instance() -> BypassHandler&
    {
        static BypassHandler instance;
        return instance;
    }

    void registerHandler(std::shared_ptr<IBypassHandler> const& handler);
    void removeHandler(std::shared_ptr<IBypassHandler> const& handler);

    static void load();
    
    auto Handler(std::string_view commandValue) -> std::shared_ptr<IBypassHandler>;

    auto size() const -> size_t;
};