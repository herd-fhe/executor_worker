#include "crypto/crypto.hpp"
#include "crypto/exception.hpp"
#include "spdlog/spdlog.h"


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
			spdlog::error("Crypto stream corrupted - wrong row size");
			throw CryptoStreamCorrupted("Crypto stream corrupted - wrong row size");
		}

		return vectors;
	}
}