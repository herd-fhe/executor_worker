#include "crypto/crypto.hpp"

#include <sstream>

#include "spdlog/spdlog.h"

#include "herd/common/model/exception.hpp"
#include "herd/common/native_type_mapping.hpp"

#include "crypto/exception.hpp"


namespace crypto
{
	CryptoVector Crypto::load_n_from_stream(std::istream& stream, std::size_t count)
	{
		CryptoVector vector(count);

		for(std::size_t i = 0; i < count; ++i)
		{
			vector[i] = load_from_stream(stream);
		}

		return vector;
	}

	std::vector<CryptoVector> Crypto::load_row_from_stream(std::istream& stream, const std::vector<unsigned int>& columns)
	{
		uint32_t row_size = 0;
		stream.read(reinterpret_cast<char*>(&row_size), sizeof(uint32_t));
		const auto pos = stream.tellg();

		std::vector<CryptoVector> vectors;
		vectors.reserve(columns.size());
		for(auto column: columns)
		{
			vectors.emplace_back(load_n_from_stream(stream, column));
		}
		const auto read_bytes = stream.tellg() - pos;
		if (read_bytes != row_size)
		{
			spdlog::error("Crypto stream corrupted - wrong row row_count");
			throw CryptoStreamCorrupted("Crypto stream corrupted - wrong row row_count");
		}

		return vectors;
	}

	void Crypto::store_vector_to_stream(std::ostream& stream, const CryptoVector& crypto_vector)
	{
		for (const auto& crypto_value: crypto_vector)
		{
			store_to_stream(stream, *crypto_value);
		}
	}

	void Crypto::store_row_to_stream(std::ostream& stream, const std::vector<CryptoVector>& row)
	{
		std::stringstream row_buffer;
		const auto start = row_buffer.tellp();

		for(const auto& crypto_vector: row)
		{
			store_vector_to_stream(row_buffer, crypto_vector);
		}

		const auto end = row_buffer.tellp();

		const auto size = static_cast<uint32_t>(end - start);
		stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
		if(stream.fail())
		{
			throw herd::common::IOWriteError("Failed to write to output data frame");
		}

		row_buffer.seekg(start);

		stream << row_buffer.rdbuf();
		if(stream.fail())
		{
			throw herd::common::IOWriteError("Failed to write to output data frame");
		}
	}

	CryptoVector Crypto::create_field_from_description(unsigned int data_width)
	{
		CryptoVector vector(data_width);

		for(std::size_t i = 0; i < data_width; ++i)
		{
			vector[i] = create_empty_value();
		}

		return vector;
	}
	std::vector<CryptoVector> Crypto::create_row_from_description(const std::vector<unsigned int>& description)
	{
		std::vector<CryptoVector> vectors;
		vectors.reserve(description.size());

		for(const auto data_type: description)
		{
			vectors.emplace_back(create_field_from_description(data_type));
		}

		return vectors;
	}
}