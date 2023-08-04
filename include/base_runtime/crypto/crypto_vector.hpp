#ifndef EXECUTOR_WORKER_CRYPTO_VECTOR_HPP
#define EXECUTOR_WORKER_CRYPTO_VECTOR_HPP

#include <memory>
#include <vector>

#include "base_runtime/crypto/crypto_value.hpp"


namespace crypto
{
	class CryptoVector
	{
	private:
		std::vector<crypto_value_ptr_t> values_;

	public:
		using const_iterator = decltype(values_)::const_iterator;

		explicit CryptoVector(std::size_t count);

		crypto_value_ptr_t& operator[](std::size_t index);
		const crypto_value_ptr_t& operator[](std::size_t index) const;

		std::size_t size() const noexcept;

		[[nodiscard]] const_iterator begin() const noexcept;
		[[nodiscard]] const_iterator cbegin() const noexcept;

		[[nodiscard]] const_iterator end() const noexcept;
		[[nodiscard]] const_iterator cend() const noexcept;

	};
}

#endif //EXECUTOR_WORKER_CRYPTO_VECTOR_HPP
