#ifndef EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP
#define EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP

#include <vector>
#include <filesystem>
#include <fstream>

#include "crypto/crypto_vector.hpp"
#include "crypto/crypto.hpp"


namespace data
{
	class DataFrameInput
	{
	public:
		DataFrameInput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns);

		std::vector<crypto::CryptoVector> read_row(crypto::Crypto& crypto);

	private:
		std::ifstream data_frame_stream_;
		std::vector<unsigned int> columns_;
	};
}

#endif //EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP
