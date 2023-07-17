#ifndef EXECUTOR_WORKER_EXECUTOR_HPP
#define EXECUTOR_WORKER_EXECUTOR_HPP

#include <thread>

#include "data_frame/data_frame_input.hpp"
#include "data_frame/data_frame_output.hpp"
#include "executor/runnable_circuit.hpp"
#include "executor/tree_runner.hpp"


class Executor
{
public:
	Executor(std::size_t worker_count=std::jthread::hardware_concurrency());

	void add_input(std::unique_ptr<data::DataFrameInput> input) noexcept;
	void set_output(std::unique_ptr<data::DataFrameOutput> output) noexcept;

	void set_crypto(std::unique_ptr<crypto::Crypto> crypto) noexcept;

	void set_circuit(RunnableCircuit circuit) noexcept;

	void map();
	void reduce();

private:
	std::shared_ptr<crypto::Crypto> crypto_;

	std::vector<std::unique_ptr<data::DataFrameInput>> inputs_;
	std::unique_ptr<data::DataFrameOutput> output_;

	RunnableCircuit circuit_;

	TreeRunner tree_runner_;
};

#endif //EXECUTOR_WORKER_EXECUTOR_HPP
