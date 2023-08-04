#include "base_runtime/data_frame/data_frame_output.hpp"

#include "cereal/cereal.hpp"

#include "herd/common/model/exception.hpp"


namespace data
{
	DataFrameOutput::DataFrameOutput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns)
	:	data_frame_stream_(data_frame_path, std::ios_base::out | std::ios_base::binary), columns_(columns)
	{
		if (data_frame_stream_.fail())
		{
			throw herd::common::FileNotExistError("Could not access data frame at path: " + data_frame_path.string());
		}
	}

	void DataFrameOutput::write_row(const std::vector<crypto::CryptoVector>& row, crypto::Crypto& crypto)
	{
		crypto.store_row_to_stream(data_frame_stream_, row);
	}
}