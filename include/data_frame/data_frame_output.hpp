#ifndef EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP
#define EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP

#include <vector>
#include <filesystem>
#include <fstream>

#include "crypto/crypto_vector.hpp"


namespace data
{
	class DataFrameOutput
	{
	public:
		DataFrameOutput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns);

		void write_row(std::vector<crypto::CryptoVector>&& row);

	private:
		std::ofstream data_frame_stream_;
		std::vector<unsigned int> columns_;
	};
}

#endif //EXECUTOR_WORKER_DATA_FRAME_OUTPUT_HPP
