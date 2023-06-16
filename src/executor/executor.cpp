#include "executor/executor.hpp"
#include "spdlog/spdlog.h"


Executor::Executor(std::size_t worker_count)
:	tree_runner_(worker_count)
{
}

void Executor::set_input(std::unique_ptr<data::DataFrameInput> input) noexcept
{
	input_ = std::move(input);
}

void Executor::set_output(std::unique_ptr<data::DataFrameOutput> output) noexcept
{
	output_ = std::move(output);
}

void Executor::set_crypto(std::unique_ptr<crypto::Crypto> crypto) noexcept
{
	crypto_ = std::move(crypto);
	tree_runner_.set_crypto(crypto_);
}

void Executor::set_circuit(RunnableCircuit circuit) noexcept
{
	circuit_ = std::move(circuit);
}

void Executor::set_row_count(uint64_t row_count)
{
	row_count_ = row_count;
}

void Executor::run()
{
	spdlog::info("Executor started");
	for(std::size_t i = 0; i < row_count_; ++i)
	{
		spdlog::info("Processing row: {}...", i);
		process_row();
		spdlog::info("Row: {} processed", i);
	}
	spdlog::info("Executor finished");
}

void Executor::process_row()
{
	auto row = input_->read_row(*crypto_);
	auto output_row = tree_runner_.execute(circuit_, std::move(row));
	circuit_.reset_circuit();
	output_->write_row(std::move(output_row));
}
