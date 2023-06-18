#ifndef EXECUTOR_WORKER_CRYPTO_VALUE_HPP
#define EXECUTOR_WORKER_CRYPTO_VALUE_HPP

#include <memory>
#include <ostream>


namespace crypto
{
	class CryptoValue
	{
	public:
		virtual ~CryptoValue() = default;
	};

	using crypto_value_ptr_t = std::unique_ptr<CryptoValue>;
	using const_crypto_value_ptr_t = std::unique_ptr<const CryptoValue>;
}

#endif //EXECUTOR_WORKER_CRYPTO_VALUE_HPP
