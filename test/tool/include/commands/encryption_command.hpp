#ifndef TEST_TOOLS_ENCRYPTION_COMMAND_HPP
#define TEST_TOOLS_ENCRYPTION_COMMAND_HPP

#include "cli/subcommand.hpp"


std::unique_ptr<cli::Subcommand> generate_encrypt_subcommand();
std::unique_ptr<cli::Subcommand> generate_decrypt_subcommand();

#endif //TEST_TOOLS_ENCRYPTION_COMMAND_HPP
