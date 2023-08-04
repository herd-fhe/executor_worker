#include "base_runtime/crypto/crypto_vector.hpp"


namespace crypto
{
	CryptoVector::CryptoVector(std::size_t count)
	{
		values_.resize(count);
	}

	crypto_value_ptr_t& CryptoVector::operator[](std::size_t index)
	{
		return values_[index];
	}

	const crypto_value_ptr_t& CryptoVector::operator[](std::size_t index) const
	{
		return values_[index];
	}

	std::size_t CryptoVector::size() const noexcept
	{
		return values_.size();
	}

	CryptoVector::const_iterator CryptoVector::begin() const noexcept
	{
		return values_.begin();
	}

	CryptoVector::const_iterator CryptoVector::cbegin() const noexcept
	{
		return values_.cbegin();
	}

	CryptoVector::const_iterator CryptoVector::end() const noexcept
	{
		return values_.end();
	}

	CryptoVector::const_iterator CryptoVector::cend() const noexcept
	{
		return values_.cend();
	}
}