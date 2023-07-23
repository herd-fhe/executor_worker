#include "commands/encryption_command.hpp"

#include <filesystem>
#include <iostream>

#include "encryption/crypto.hpp"


namespace
{
	constexpr const char* const context_path_name = "context";
	constexpr const char* const private_key_path_name = "key";
	constexpr const char* const data_out_path_name = "data_out";
	constexpr const char* const data_in_path_name = "data_in";
	constexpr const char* const row_width_name = "width";
	constexpr const char* const row_count_name = "count";
}


std::unique_ptr<cli::Subcommand> generate_encrypt_subcommand()
{
	auto subcommand = std::make_unique<cli::Subcommand>(
			"encrypt",
			[](const cli::ParameterProxy& proxy)
			{
				const std::filesystem::path context = proxy.string(context_path_name);
				const std::filesystem::path private_key = proxy.string(private_key_path_name);
				const std::filesystem::path out_path = proxy.string(data_out_path_name);
				auto row_width = proxy.num<std::size_t>(row_width_name);
				auto row_count = proxy.num<std::size_t>(row_count_name);

				std::vector<std::vector<bool>> data;
				for(std::size_t i = 0; i < row_count; ++i)
				{
					std::string temp_line;
					std::getline(std::cin, temp_line);

					if(temp_line.size() != row_width)
					{
						throw std::runtime_error("All rows must width equal " + std::to_string(i));
					}

					auto& row = data.emplace_back();
					for(const char sym: temp_line)
					{
						switch (sym)
						{
							case '0':
								row.emplace_back(false);
								break;
							case '1':
								row.emplace_back(true);
								break;
							default:
								throw std::runtime_error("Unsupported symbol in row (only 0, 1 supported)");
						}
					}
				}

				encrypt(context, private_key, out_path, data);

				return 0;
			}
	);
	subcommand->set_description("Encrypt provided data");

	auto context_path = std::make_unique<cli::Parameter>(context_path_name);
	context_path->set_description("Context file (cloud keys, context settings)");
	subcommand->register_parameter(std::move(context_path));

	auto private_key_path = std::make_unique<cli::Parameter>(private_key_path_name);
	private_key_path->set_description("Private key file");
	subcommand->register_parameter(std::move(private_key_path));

	auto data_out_path = std::make_unique<cli::Parameter>(data_out_path_name);
	data_out_path->set_description("File to store encrypted data");
	subcommand->register_parameter(std::move(data_out_path));

	auto row_width = std::make_unique<cli::Parameter>(row_width_name);
	row_width->set_description("Row width in bits");
	subcommand->register_parameter(std::move(row_width));

	auto row_count = std::make_unique<cli::Parameter>(row_count_name);
	row_count->set_description("Row count");
	subcommand->register_parameter(std::move(row_count));

	return subcommand;
}

std::unique_ptr<cli::Subcommand> generate_decrypt_subcommand()
{
	auto subcommand = std::make_unique<cli::Subcommand>(
			"decrypt",
			[](const cli::ParameterProxy& proxy)
			{
				const std::filesystem::path context = proxy.string(context_path_name);
				const std::filesystem::path private_key = proxy.string(private_key_path_name);
				const std::filesystem::path in_path = proxy.string(data_in_path_name);
				auto row_width = proxy.num<std::size_t>(row_width_name);
				auto row_count = proxy.num<std::size_t>(row_count_name);

				decrypt(context, private_key, in_path, row_width, row_count);

				return 0;
			}
	);
	subcommand->set_description("Decrypt provided data");

	auto context_path = std::make_unique<cli::Parameter>(context_path_name);
	context_path->set_description("Context file (cloud keys, context settings)");
	subcommand->register_parameter(std::move(context_path));

	auto private_key_path = std::make_unique<cli::Parameter>(private_key_path_name);
	private_key_path->set_description("Private key file");
	subcommand->register_parameter(std::move(private_key_path));

	auto data_out_path = std::make_unique<cli::Parameter>(data_in_path_name);
	data_out_path->set_description("Encrypted data file");
	subcommand->register_parameter(std::move(data_out_path));

	auto row_width = std::make_unique<cli::Parameter>(row_width_name);
	row_width->set_description("Row width in bits");
	subcommand->register_parameter(std::move(row_width));

	auto row_count = std::make_unique<cli::Parameter>(row_count_name);
	row_count->set_description("Row count");
	subcommand->register_parameter(std::move(row_count));

	return subcommand;
}
