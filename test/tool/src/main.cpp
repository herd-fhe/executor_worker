#include "cli/command_line.hpp"

#include "commands/key_command.hpp"
#include "commands/encryption_command.hpp"

int main(int argc, char** argv)
{
	cli::CommandLine command_line;

	command_line.add_subcommand(generate_keys_subcommand());
	command_line.add_subcommand(generate_encrypt_subcommand());
	command_line.add_subcommand(generate_decrypt_subcommand());

	return command_line.execute(argc, argv);
}