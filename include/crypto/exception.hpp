#ifndef EXECUTOR_WORKER_CRYPTO_EXCEPTION_HPP
#define EXECUTOR_WORKER_CRYPTO_EXCEPTION_HPP

#include <stdexcept>


namespace crypto
{
	struct CryptoException: public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	struct CryptoStreamCorrupted: public CryptoException
	{
		using CryptoException::CryptoException;
	};

	struct OperationNotSupported: public CryptoException
	{
		using CryptoException::CryptoException;
	};
}

#endif //EXECUTOR_WORKER_CRYPTO_EXCEPTION_HPP
