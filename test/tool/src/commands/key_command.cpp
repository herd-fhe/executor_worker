#include "commands/key_command.hpp"

#include <filesystem>

#include "encryption/key.hpp"


namespace
{
	constexpr const char* const context_path_name = "context_out";
	constexpr const char* const private_key_path_name = "key_out";
	constexpr const char* const demo_name = "demo";
}


std::unique_ptr<cli::Subcommand> generate_keys_subcommand()
{
	auto subcommand = std::make_unique<cli::Subcommand>(
			"generate-keys",
			[](const cli::ParameterProxy& proxy)
			{
				const std::filesystem::path context = proxy.string(context_path_name);
				const std::filesystem::path private_key = proxy.string(private_key_path_name);
				generate_key(context, private_key, proxy.boolean(demo_name));

				return 0;
			}
	);
	subcommand->set_description("Generate context & private key");

	auto context_path = std::make_unique<cli::Parameter>(context_path_name);
	context_path->set_description("File to store context (cloud keys, context settings)");
	subcommand->register_parameter(std::move(context_path));

	auto private_key_path = std::make_unique<cli::Parameter>(private_key_path_name);
	private_key_path->set_description("File to store generated private key");
	subcommand->register_parameter(std::move(private_key_path));

	auto demo = std::make_unique<cli::Parameter>(demo_name, true);
	demo->set_description("Use TOY version of encryption (Use only for debug)");
	subcommand->register_parameter(std::move(demo));

	return subcommand;
}