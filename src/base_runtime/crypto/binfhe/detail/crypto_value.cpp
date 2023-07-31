#include "base_runtime/crypto/binfhe/detail/crypto_value.hpp"

#include <binfhecontext-ser.h>


namespace crypto::binfhe::detail
{
	CryptoValueImpl::CryptoValueImpl(lbcrypto::ConstLWECiphertext&& ciphertext)
	:	ciphertext_(ciphertext)
	{
	}

	lbcrypto::ConstLWECiphertext CryptoValueImpl::value()
	{
		return ciphertext_;
	}

	std::shared_ptr<const lbcrypto::LWECiphertextImpl> CryptoValueImpl::value() const
	{
		return ciphertext_;
	}
}