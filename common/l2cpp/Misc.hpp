/// @author    Chnossos
/// @date      Created on 2026-02-16

#pragma once

#include <memory>
#include <string>

namespace l2cpp
{
    auto hexdump(void const * ptr, size_t size) -> std::string;

    template<typename T, typename D = std::default_delete<T>>
    auto makeUPtr(T * t, D deleter = {}) -> decltype(auto)
    {
        return std::unique_ptr<T, D>(t, deleter);
    }
}
