#include "crypto/binfhe/detail/crypto_value.hpp"


namespace crypto::binfhe::detail
{
	CryptoValueImpl::CryptoValueImpl(lbcrypto::LWECiphertext&& ciphertext)
	:	ciphertext_(std::move(ciphertext))
	{
	}

	lbcrypto::LWECiphertext CryptoValueImpl::value()
	{
		return ciphertext_;
	}

	std::shared_ptr<const lbcrypto::LWECiphertextImpl> CryptoValueImpl::value() const
	{
		return ciphertext_;
	}

	void CryptoValueImpl::serialize(std::ostream& out_stream) const
	{
		static_cast<void>(out_stream);
	}
}