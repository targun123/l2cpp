/// @author    Chnossos
/// @date      Created on 2026-02-16

#pragma once

#include <memory>
#include <span>
#include <string>

#include "Typedefs.hpp"

namespace l2cpp
{
    auto hexdump(void const * ptr, size_t size) -> std::string;
    inline auto hexdump(std::span<byte const> const buffer) -> std::string {
        return hexdump(buffer.data(), buffer.size());
    }

    template<typename T, typename D = std::default_delete<T>>
    auto makeUPtr(T * t, D deleter = {}) -> decltype(auto) {
        return std::unique_ptr<T, D>(t, deleter);
    }
}
