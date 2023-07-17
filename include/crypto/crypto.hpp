#ifndef EXECUTOR_WORKER_CRYPTO_HPP
#define EXECUTOR_WORKER_CRYPTO_HPP

#include <cstddef>
#include <memory>
#include <istream>

#include "herd/common/model/circuit/operation.hpp"
#include "herd/common/model/data_type.hpp"

#include "crypto/crypto_value.hpp"
#include "crypto/crypto_vector.hpp"


namespace crypto
{
	class Crypto
	{
	public:
		virtual ~Crypto() = default;

		virtual crypto_value_ptr_t load_from_stream(std::istream& stream) = 0;
		CryptoVector load_n_from_stream(std::istream& stream, std::size_t count);
		std::vector<CryptoVector> load_row_from_stream(std::istream& stream, const std::vector<unsigned int>& columns);

		virtual crypto_value_ptr_t create_empty_value() = 0;
		CryptoVector create_field_from_description(unsigned int data_width);
		std::vector<CryptoVector> create_row_from_description(const std::vector<unsigned int>& description);

		virtual void store_to_stream(std::ostream& stream, const CryptoValue& value) = 0;
		void store_vector_to_stream(std::ostream& stream, const CryptoVector& crypto_vector);
		void store_row_to_stream(std::ostream& stream, const std::vector<CryptoVector>& row);

		virtual crypto_value_ptr_t ternary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& crypto_b, const CryptoValue& crypto_c) = 0;
		virtual crypto_value_ptr_t binary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& crypto_b) = 0;
		virtual crypto_value_ptr_t unary_op(herd::common::Operation operation, const CryptoValue& input) = 0;

		virtual crypto_value_ptr_t constant(bool value) = 0;
		virtual crypto_value_ptr_t copy(const CryptoValue& input) = 0;
	};
}

#endif //EXECUTOR_WORKER_CRYPTO_HPP
