#include "base_runtime/data_frame/data_frame_output.hpp"

#include "cereal/cereal.hpp"

#include "herd/common/model/exception.hpp"


#include <cassert>


namespace
{
	std::filesystem::path get_temporary_path(const std::filesystem::path& data_frame_path)
	{
		const auto& filename = data_frame_path.filename();
		const auto temp_filename = filename.string() + "-tmp";

		auto new_path = data_frame_path;
		new_path.replace_filename(temp_filename);

		return new_path;
	}
}

namespace data
{
	DataFrameOutput::DataFrameOutput(const std::filesystem::path& data_frame_path, const std::vector<unsigned int>& columns)
	:	data_frame_path_(data_frame_path), data_frame_stream_(get_temporary_path(data_frame_path), std::ios_base::out | std::ios_base::binary), columns_(columns)
	{
		if (data_frame_stream_.fail())
		{
			throw herd::common::FileNotExistError("Could not access data frame at path: " + data_frame_path.string());
		}
	}

	void DataFrameOutput::write_row(const std::vector<crypto::CryptoVector>& row, crypto::Crypto& crypto)
	{
		assert(!finished_);
		crypto.store_row_to_stream(data_frame_stream_, row);
	}

	void DataFrameOutput::finish()
	{
		finished_ = true;
		if(data_frame_stream_.is_open())
		{
			data_frame_stream_.flush();
			data_frame_stream_.close();
		}

		std::filesystem::rename(get_temporary_path(data_frame_path_), data_frame_path_);
	}
} // namespace data