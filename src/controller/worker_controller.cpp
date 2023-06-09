#include "controller/worker_controller.hpp"

#include <spdlog/spdlog.h>

#include "herd/mapper/worker.hpp"

#include "crypto/crypto_factory.hpp"
#include "data_frame/data_frame.hpp"
#include "executor/exception.hpp"
#include "executor/executor.hpp"
#include "herd/common/model/exception.hpp"
#include "mapper/circuit.hpp"


WorkerController::WorkerController(Config config)
:	config_(std::move(config))
{
}

grpc::Status WorkerController::map([[maybe_unused]] ::grpc::ServerContext* context, const ::herd::proto::MapTask* request, [[maybe_unused]] ::herd::proto::Empty* response)
{
	herd::common::CryptoKeyPtr crypto_key_ptr{};
	herd::common::InputDataFramePtr input_data_frame_ptr{};
	herd::common::DataFramePtr output_data_frame_ptr{};
	herd::common::UUID session_uuid{};
	RunnableCircuit circuit{};

	try
	{
		crypto_key_ptr = herd::mapper::to_model(request->crypto_key_ptr());
		input_data_frame_ptr = herd::mapper::to_model(request->input_data_frame_ptr());
		output_data_frame_ptr = herd::mapper::to_model(request->output_data_frame_ptr());
		session_uuid = herd::common::UUID(request->session_uuid());
		circuit = mapper::to_model(request->circuit());
	}
	catch(const std::runtime_error& error)
	{
		spdlog::error(error.what());
		return { grpc::INVALID_ARGUMENT, error.what() };
	}

	std::unique_ptr<crypto::Crypto> crypto;

	std::unique_ptr<data::DataFrameInput> input_data_frame{};
	std::unique_ptr<data::DataFrameOutput> output_data_frame{};
	try
	{
		crypto = crypto::CryptoFactory::create_crypto(crypto_key_ptr, session_uuid, config_.key_base_dir);
		input_data_frame = data::load_input(input_data_frame_ptr, circuit.input, session_uuid, config_.storage_base_dir);
		output_data_frame = data::load_output(output_data_frame_ptr, circuit.output, session_uuid, config_.storage_base_dir);
	}
	catch(const herd::common::IOError& error)
	{
		spdlog::error(error.what());
		return {grpc::FAILED_PRECONDITION, error.what()};
	}

	spdlog::info("Input data frame: {}:{}", input_data_frame_ptr.pointer.uuid.as_string(), input_data_frame_ptr.pointer.partition);
	spdlog::info("Output data frame: {}:{}", output_data_frame_ptr.uuid.as_string(), output_data_frame_ptr.partition);
	spdlog::info("Loaded task requirements (input data frame, cloud key)");

	auto executor = Executor(1);
	executor.set_crypto(std::move(crypto));

	executor.add_input(std::move(input_data_frame));
	executor.set_output(std::move(output_data_frame));

	executor.set_circuit(std::move(circuit));

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
