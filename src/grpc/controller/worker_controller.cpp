#include "grpc/controller/worker_controller.hpp"

#include <spdlog/spdlog.h>

#include "herd/mapper/worker.hpp"

#include "base_runtime/crypto/crypto_factory.hpp"
#include "base_runtime/data_frame/data_frame.hpp"
#include "base_runtime/executor/exception.hpp"
#include "base_runtime/executor/executor.hpp"
#include "herd/common/model/exception.hpp"
#include "grpc/mapper/circuit.hpp"


WorkerController::WorkerController(Config config)
:	config_(std::move(config))
{
}

grpc::Status WorkerController::map([[maybe_unused]] grpc::ServerContext* context, const herd::proto::MapTask* request, [[maybe_unused]] herd::proto::Empty* response)
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

	auto executor = Executor();
	spdlog::info("Starting Map task");
	spdlog::info("Input data frame: {}:{}", input_data_frame_ptr.pointer.uuid.as_string(), input_data_frame_ptr.pointer.partition);
	spdlog::info("Output data frame: {}:{}", output_data_frame_ptr.uuid.as_string(), output_data_frame_ptr.partition);

	try
	{
		executor.set_crypto(crypto::CryptoFactory::create_crypto(crypto_key_ptr, session_uuid, config_.key_base_dir));
		executor.add_input(data::load_input(input_data_frame_ptr, circuit.inputs[0], session_uuid, config_.storage_base_dir));
		executor.set_output(data::load_output(output_data_frame_ptr, circuit.output, session_uuid, config_.storage_base_dir));
	}
	catch(const herd::common::IOError& error)
	{
		spdlog::error(error.what());
		return {grpc::FAILED_PRECONDITION, error.what()};
	}

	executor.set_circuit(std::move(circuit));

	spdlog::info("Loaded task requirements (input data frame, cloud key)");

	try
	{
		executor.map();
	}
	catch(const ExecutorException& exception)
	{
		spdlog::error(exception.what());

		return {grpc::INTERNAL, exception.what()};
	}

	return grpc::Status::OK;
}

grpc::Status WorkerController::reduce([[maybe_unused]] grpc::ServerContext* context, const herd::proto::ReduceTask* request, [[maybe_unused]] herd::proto::Empty* response)
{
	herd::common::CryptoKeyPtr crypto_key_ptr{};
	std::vector<herd::common::InputDataFramePtr> input_data_frame_ptrs{};
	herd::common::DataFramePtr output_data_frame_ptr{};
	herd::common::UUID session_uuid{};
	RunnableCircuit circuit{};

	try
	{
		crypto_key_ptr = herd::mapper::to_model(request->crypto_key_ptr());

		input_data_frame_ptrs.reserve(static_cast<std::size_t>(request->input_data_frame_ptrs_size()));
		for(const auto& input_data_frame_ptr_proto: request->input_data_frame_ptrs())
		{
			input_data_frame_ptrs.emplace_back(herd::mapper::to_model(input_data_frame_ptr_proto));
		}

		output_data_frame_ptr = herd::mapper::to_model(request->output_data_frame_ptr());
		session_uuid = herd::common::UUID(request->session_uuid());
		circuit = mapper::to_model(request->circuit());
	}
	catch(const std::runtime_error& error)
	{
		spdlog::error(error.what());
		return { grpc::INVALID_ARGUMENT, error.what() };
	}

	auto executor = Executor();

	spdlog::info("Starting Reduce task");
	spdlog::info("Output data frame: {}:{}", output_data_frame_ptr.uuid.as_string(), output_data_frame_ptr.partition);

	try
	{
		executor.set_crypto(crypto::CryptoFactory::create_crypto(crypto_key_ptr, session_uuid, config_.key_base_dir));
		for(std::size_t i = 0; i < input_data_frame_ptrs.size(); ++i)
		{
			executor.add_input(data::load_input(input_data_frame_ptrs[i], circuit.inputs[i], session_uuid, config_.storage_base_dir));
		}
		executor.set_output(data::load_output(output_data_frame_ptr, circuit.output, session_uuid, config_.storage_base_dir));
	}
	catch(const herd::common::IOError& error)
	{
		spdlog::error(error.what());
		return {grpc::FAILED_PRECONDITION, error.what()};
	}

	executor.set_circuit(std::move(circuit));
	spdlog::info("Loaded task requirements (input data frame, cloud key)");

	try
	{
		executor.reduce();
	}
	catch(const ExecutorException& exception)
	{
		spdlog::error(exception.what());

		return {grpc::INTERNAL, exception.what()};
	}

	return grpc::Status::OK;
}
