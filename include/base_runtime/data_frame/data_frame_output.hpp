#ifndef EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP
#define EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP

#include <vector>
#include <filesystem>
#include <fstream>

#include "base_runtime/crypto/crypto.hpp"
#include "base_runtime/crypto/crypto_vector.hpp"


namespace data
{
	class DataFrameOutput
	{
	public:
		DataFrameOutput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns);

		void write_row(const std::vector<crypto::CryptoVector>& row, crypto::Crypto& crypto);
		void finish();

	private:
		std::filesystem::path data_frame_path_;
		std::ofstream data_frame_stream_;

		bool finished_ = false;
		std::vector<unsigned int> columns_;
	};
}

#endif //EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP
