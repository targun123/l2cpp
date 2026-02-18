/// @author    Chnossos
/// @date      Created on 2026-02-18

#pragma once

// Project includes
#include "Pimpl.hpp"
#include "Typedefs.hpp"

// C++ includes
#include <span>

namespace l2cpp { class Blowfish; }

class l2cpp::Blowfish
{
public:
    explicit Blowfish(std::span<byte const> key);
    ~Blowfish();

public:
    size_t encrypt(std::span<byte> data) const;
    size_t decrypt(std::span<byte> data) const;

private:
    struct BlowfishImpl;
    Pimpl<BlowfishImpl> _pimpl;
};
