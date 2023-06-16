#include "data_frame/data_frame_input.hpp"


namespace data
{
	DataFrameInput::DataFrameInput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns)
	: data_frame_stream_(data_frame_path, std::ios_base::in | std::ios_base::binary), columns_(columns)
	{
	}

	std::vector<crypto::CryptoVector> DataFrameInput::read_row(crypto::Crypto& crypto)
	{
		return crypto.load_row_from_stream(data_frame_stream_, columns_);
	}
}