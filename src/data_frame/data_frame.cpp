#include "data_frame/data_frame.hpp"


namespace data
{
	std::unique_ptr<DataFrameInput> load_input(const herd::common::InputDataFramePtr& ptr, const std::vector<unsigned int>& columns, const herd::common::UUID& session_uuid, const std::filesystem::path& storage_base_dir)
	{
		const auto data_frame_path = storage_base_dir / session_uuid.as_string() / ptr.pointer.uuid.as_string() / std::to_string(ptr.pointer.partition);
		return std::make_unique<DataFrameInput>(data_frame_path, columns, ptr.row_count);
	}

	std::unique_ptr<DataFrameOutput> load_output(const herd::common::DataFramePtr& ptr, const std::vector<unsigned int>& columns, const herd::common::UUID& session_uuid, const std::filesystem::path& storage_base_dir)
	{
		const auto data_frame_path = storage_base_dir / session_uuid.as_string() / ptr.uuid.as_string() / std::to_string(ptr.partition);
		return std::make_unique<DataFrameOutput>(data_frame_path, columns);
	}
}