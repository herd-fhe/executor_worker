#ifndef EXECUTOR_WORKER_BINFHE_CRYPTO_VALUE_HPP
#define EXECUTOR_WORKER_BINFHE_CRYPTO_VALUE_HPP

#include <binfhecontext.h>

#include "crypto/crypto_value.hpp"


namespace crypto::binfhe::detail
{
	class CryptoValueImpl final: public CryptoValue
	{
	public:
		explicit CryptoValueImpl(lbcrypto::ConstLWECiphertext&& ciphertext);

		[[nodiscard]] lbcrypto::ConstLWECiphertext value();
		[[nodiscard]] std::shared_ptr<const lbcrypto::LWECiphertextImpl> value() const;

		void serialize(std::ostream& out_stream) const override;

	private:
		lbcrypto::ConstLWECiphertext ciphertext_;
	};
}

#endif //EXECUTOR_WORKER_BINFHE_CRYPTO_VALUE_HPP
