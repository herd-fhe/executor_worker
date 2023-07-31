#include "base_runtime/data_frame/data_frame_input.hpp"

#include "herd/common/model/exception.hpp"


namespace data
{
	DataFrameInput::DataFrameInput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns, uint64_t row_count)
	: data_frame_stream_(data_frame_path, std::ios_base::in | std::ios_base::binary), columns_(columns), row_count_(row_count)
	{
		if (data_frame_stream_.fail())
		{
			throw herd::common::FileNotExistError("Could not access data frame at path: " + data_frame_path.string());
		}
	}

	std::vector<crypto::CryptoVector> DataFrameInput::read_row(crypto::Crypto& crypto)
	{
		return crypto.load_row_from_stream(data_frame_stream_, columns_);
	}

	uint64_t DataFrameInput::row_count() const noexcept
	{
		return row_count_;
	}
}