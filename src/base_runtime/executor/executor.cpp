#include "base_runtime/executor/executor.hpp"
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

void Executor::map()
{
	spdlog::info("Executor started - map");
	for(auto const& input: inputs_)
	{
		for(uint64_t i = 0; i < input->row_count(); ++i)
		{
			spdlog::info("Processing row: {}...", std::to_string(i));
			std::vector<std::vector<crypto::CryptoVector>> input_arguments;
			input_arguments.emplace_back(input->read_row(*crypto_));

			auto output_row = tree_runner_.execute(circuit_, std::move(input_arguments));
			circuit_.reset_circuit();
			output_->write_row(output_row, *crypto_);
			spdlog::info("Row: {} processed", i);
		}
	}

	output_->finish();
	spdlog::info("Executor finished - map");
}

void Executor::reduce()
{
	spdlog::info("Executor started - reduce");

	auto aggregated_value = crypto_->create_row_from_description(circuit_.output);

	for(auto const& input: inputs_)
	{
		for(uint64_t i = 0; i < input->row_count(); ++i)
		{
			spdlog::info("Processing row: {}...", std::to_string(i));
			std::vector<std::vector<crypto::CryptoVector>> input_arguments;
			input_arguments.emplace_back(std::move(aggregated_value));
			input_arguments.emplace_back(input->read_row(*crypto_));

			aggregated_value = tree_runner_.execute(circuit_, std::move(input_arguments));
			circuit_.reset_circuit();
			spdlog::info("Row: {} processed", i);
		}
	}
	output_->write_row(aggregated_value, *crypto_);

	output_->finish();
	spdlog::info("Executor finished - reduce");
}
