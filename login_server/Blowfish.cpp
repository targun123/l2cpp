/// @author    Chnossos
/// @date      Created on 2026-02-18

#include "Blowfish.hpp"

// Project includes
#include <l2cpp/details/Pimpl.hpp>

// Third-party includes
#include <openssl/blowfish.h>

// System includes
#ifdef WIN32
# include <winsock2.h>
#endif

using l2cpp::Blowfish;

struct Blowfish::BlowfishImpl
{
    BF_KEY key;

    void apply(std::span<byte>, bool) const;
};

Blowfish::Blowfish(std::span<byte const> const key)
{
    BF_set_key(&_pimpl->key, static_cast<int>(key.size()), key.data());
}

Blowfish::~Blowfish() = default;

void Blowfish::encrypt(std::span<byte> data) const
{
    _pimpl->apply(std::move(data), BF_ENCRYPT);
}

void Blowfish::decrypt(std::span<byte> data) const
{
    _pimpl->apply(std::move(data), BF_DECRYPT);
}

void Blowfish::BlowfishImpl::apply(std::span<byte> const s, bool const mode) const
{
    constexpr size_t chunkSize = sizeof(u64);

    if (s.size() < chunkSize)
        return;

    union { u32 ints[2]; byte raw[sizeof(ints)]; } chunk;
    for (size_t i = 0; i < s.size(); i += chunkSize)
    {
        chunk.ints[0] = htonl(*reinterpret_cast<u32 const *>(s.data() + i + 0));
        chunk.ints[1] = htonl(*reinterpret_cast<u32 const *>(s.data() + i + 4));

        BF_ecb_encrypt(chunk.raw, chunk.raw, &key, mode);

        *reinterpret_cast<u32 *>(s.data() + i + 0) = ntohl(chunk.ints[0]);
        *reinterpret_cast<u32 *>(s.data() + i + 4) = ntohl(chunk.ints[1]);
    }
}
