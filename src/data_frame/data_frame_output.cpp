#include "data_frame/data_frame_output.hpp"


namespace data
{
	DataFrameOutput::DataFrameOutput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns)
	:	data_frame_stream_(data_frame_path, std::ios_base::out | std::ios_base::binary), columns_(columns)
	{
	}

	void DataFrameOutput::write_row(std::vector<crypto::CryptoVector>&& row)
	{
		std::stringstream row_buffer;
		const auto start = row_buffer.tellp();

		for(const auto& crypto_vector: row)
		{
			for(const auto& value: crypto_vector)
			{
				value->serialize(row_buffer);
			}
		}
		const auto end = row_buffer.tellp();

		const auto size = static_cast<uint32_t>(end - start);
		data_frame_stream_.write(reinterpret_cast<const char*>(&size), sizeof(size));

		row_buffer.seekg(start);

		data_frame_stream_ << row_buffer.rdbuf();
	}
}