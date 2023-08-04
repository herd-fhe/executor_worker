#ifndef EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP
#define EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP

#include <vector>
#include <filesystem>
#include <fstream>

#include "base_runtime/crypto/crypto_vector.hpp"
#include "base_runtime/crypto/crypto.hpp"


namespace data
{
	class DataFrameInput
	{
	public:
		DataFrameInput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns, uint64_t row_count);

		std::vector<crypto::CryptoVector> read_row(crypto::Crypto& crypto);
		uint64_t row_count() const noexcept;
	private:
		std::ifstream data_frame_stream_;
		std::vector<unsigned int> columns_;

		uint64_t row_count_;
	};
}

#endif //EXECUTOR_WORKER_DATA_FRAME_INPUT_HPP
