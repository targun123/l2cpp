/// @author    Chnossos
/// @date      Created on 2026-02-18

#pragma once

// C++ includes
#include <memory>

template<typename T, typename D = std::default_delete<T>>
class Pimpl
{
public:
    explicit Pimpl(T * impl)                   noexcept: _impl(impl)                     {}
             Pimpl(void * impl, D deleter)     noexcept: _impl(impl, std::move(deleter)) {}
    explicit Pimpl(std::unique_ptr<T, D> impl) noexcept: _impl(std::move(impl))          {}

public:
    auto operator->()       -> T *       { return static_cast<T *      >(_impl.get()); }
    auto operator->() const -> T const * { return static_cast<T const *>(_impl.get()); }

private:
    std::unique_ptr<T, D> _impl;
};
