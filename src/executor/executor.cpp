#include "executor/executor.hpp"
#include "spdlog/spdlog.h"


Executor::Executor(std::size_t worker_count)
:	tree_runner_(worker_count)
{
}

void Executor::add_input(std::unique_ptr<data::DataFrameInput> input) noexcept
{
	inputs_.emplace_back(std::move(input));
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

void Executor::run()
{
	spdlog::info("Executor started");
	for(auto& input: inputs_)
	{
		for(uint64_t i = 0; i < input->row_count(); ++i)
		{
			spdlog::info("Processing row: {}...", std::to_string(i));
			auto row = input->read_row(*crypto_);
			auto output_row = tree_runner_.execute(circuit_, std::move(row));
			circuit_.reset_circuit();
			output_->write_row(output_row, *crypto_);
			spdlog::info("Row: {} processed", i);
		}
	}
	spdlog::info("Executor finished");
}
