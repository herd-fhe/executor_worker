#ifndef EXECUTOR_WORKER_DATA_FRAME_HPP
#define EXECUTOR_WORKER_DATA_FRAME_HPP

#include <vector>
#include <filesystem>

#include "herd/common/model/worker/data_frame_ptr.hpp"
#include "herd/common/uuid.hpp"

#include "data_frame_input.hpp"
#include "data_frame_output.hpp"


namespace data
{
	std::unique_ptr<DataFrameInput> load_input(const herd::common::DataFramePtr& ptr, const std::vector<unsigned int>& columns, const herd::common::UUID& session_uuid, const std::filesystem::path& storage_base_dir);
	std::unique_ptr<DataFrameOutput> load_output(const herd::common::DataFramePtr& ptr, const std::vector<unsigned int>& columns, const herd::common::UUID& session_uuid, const std::filesystem::path& storage_base_dir);
}

#endif //EXECUTOR_WORKER_DATA_FRAME_HPP
