#ifndef EXECUTOR_WORKER_BINFHE_CRYPTO_HPP
#define EXECUTOR_WORKER_BINFHE_CRYPTO_HPP

#include "crypto/crypto.hpp"
#include "crypto/crypto_value.hpp"
#include "crypto/i_keyset.hpp"
#include "keyset.hpp"


namespace crypto::binfhe::detail
{
	class CryptoImpl final: public Crypto
	{
	public:
		explicit CryptoImpl(std::unique_ptr<IKeyset> keyset);

		crypto_value_ptr_t load_from_stream(std::istream& stream) override;
		void store_to_stream(std::ostream& stream, const CryptoValue& value) override;

		crypto_value_ptr_t create_empty_value() override;

		crypto_value_ptr_t ternary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& input_b, const CryptoValue& input_c) override;
		crypto_value_ptr_t binary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& input_b) override;
		crypto_value_ptr_t unary_op(herd::common::Operation operation, const CryptoValue& input) override;

		crypto_value_ptr_t constant(bool value) override;
		crypto_value_ptr_t copy(const CryptoValue& input) override;

	private:
		std::unique_ptr<IKeyset> keyset_holder_;
		KeysetImpl& keyset_;
	};
}

#endif //EXECUTOR_WORKER_BINFHE_CRYPTO_HPP
