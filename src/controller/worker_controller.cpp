#include "controller/worker_controller.hpp"

#include <spdlog/spdlog.h>

#include "herd/mapper/worker.hpp"

#include "crypto/crypto_factory.hpp"
#include "data_frame/data_frame.hpp"
#include "executor/exception.hpp"
#include "executor/executor.hpp"
#include "mapper/circuit.hpp"


WorkerController::WorkerController(Config config)
:	config_(std::move(config))
{
}

grpc::Status WorkerController::run([[maybe_unused]] ::grpc::ServerContext* context, const ::herd::proto::Task* request, [[maybe_unused]] ::herd::proto::Empty* response)
{
	herd::common::CryptoKeyPtr crypto_key_ptr{};
	herd::common::DataFramePtr input_data_frame_ptr{};
	herd::common::DataFramePtr output_data_frame_ptr{};
	herd::common::UUID session_uuid{};
	uint64_t row_count = 0;

	try
	{
		crypto_key_ptr = herd::mapper::to_model(request->crypto_key_ptr());
		input_data_frame_ptr = herd::mapper::to_model(request->input_data_frame_ptr());
		output_data_frame_ptr = herd::mapper::to_model(request->output_data_frame_ptr());
		session_uuid = herd::common::UUID(request->session_uuid());
		row_count = request->row_count();
	}
	catch(const std::runtime_error& error)
	{
		spdlog::error(error.what());
		return grpc::Status(grpc::INVALID_ARGUMENT, error.what());
	}

	auto crypto = crypto::CryptoFactory::create_crypto(crypto_key_ptr, session_uuid, config_.key_base_dir);
	auto circuit = mapper::to_model(request->circuit());
	auto input_data_frame = data::load_input(input_data_frame_ptr, circuit.input, session_uuid, config_.storage_base_dir);
	auto output_data_frame = data::load_output(output_data_frame_ptr, circuit.output, session_uuid, config_.storage_base_dir);

	auto executor = Executor();
	executor.set_crypto(std::move(crypto));

	executor.set_input(std::move(input_data_frame));
	executor.set_output(std::move(output_data_frame));

	executor.set_circuit(std::move(circuit));
	executor.set_row_count(row_count);

	try
	{
		executor.run();
	}
	catch(const ExecutorException& exception)
	{
		spdlog::error(exception.what());

		return grpc::Status(grpc::INTERNAL, exception.what());
	}

	return grpc::Status::OK;
}
