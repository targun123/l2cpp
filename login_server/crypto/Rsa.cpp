/// @author    Chnossos
/// @date      Created on 2026-04-11

#include "Rsa.hpp"

// Project includes
#include "../Constants.hpp"

#include <l2cpp/Exception.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <l2cpp/details/Pimpl.hpp>

// Third-party includes
#include <openssl/err.h>
#include <openssl/rsa.h>

struct Rsa::RsaImpl
{
    std::unique_ptr<RSA,    decltype(&RSA_free)> rsaKey;
    std::unique_ptr<BIGNUM, decltype(&BN_free)>  bigNum;
    std::vector<byte>                            modulus;

    explicit RsaImpl(std::string_view exponent);

private:
    static auto initBigNum(std::string_view exponent) -> BIGNUM *;
};

template class Pimpl<Rsa::RsaImpl>;

Rsa::RsaImpl::RsaImpl(std::string_view const exponent)
    : rsaKey(RSA_new(), &RSA_free)
    , bigNum(initBigNum(exponent), &BN_free)
{
    RSA_generate_key_ex(rsaKey.get(), Constants::rsaKeyBitSize, bigNum.get(), nullptr);

    BIGNUM const * n = nullptr;
    RSA_get0_key(rsaKey.get(), &n, nullptr, nullptr);
    modulus.resize(BN_num_bytes(n));
    BN_bn2bin(n, modulus.data());

    // scramble modulus
    for (size_t i = 0; i < 0x04; ++i) std::swap(modulus[i], modulus[0x4d + i]);
    for (size_t i = 0; i < 0x40; ++i) modulus[0x00 + i] ^= modulus[0x40 + i];
    for (size_t i = 0; i < 0x04; ++i) modulus[0x0d + i] ^= modulus[0x34 + i];
    for (size_t i = 0; i < 0x40; ++i) modulus[0x40 + i] ^= modulus[0x00 + i];
}

auto Rsa::RsaImpl::initBigNum(std::string_view const exponent) -> BIGNUM *
{
    BIGNUM * n = nullptr;
    BN_dec2bn(&n, exponent.data());
    return n;
}

// ---------------------------------------------------------------------------------------------------------------------

Rsa::Rsa(std::string_view exponent)
    : _impl(exponent)
{}

Rsa::~Rsa() = default;

auto Rsa::instance() -> Rsa &
{
    static Rsa rsa(Constants::rsaExponent);
    return rsa;
}

auto Rsa::modulus() const -> std::span<byte const>
{
    return _impl->modulus;
}

void Rsa::decrypt(std::span<byte> const buffer)
{
    while (ERR_get_error()) {} // Empty the OpenSSL error queue

    auto const key = _impl->rsaKey.get();
    RSA_private_decrypt(RSA_size(key), buffer.data(), buffer.data(), key, RSA_NO_PADDING);

    auto const error = ERR_get_error();
    L2CPP_BC_ASSERT(!error, error, "{}", ERR_error_string(error, nullptr));
}
