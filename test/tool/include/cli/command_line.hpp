#ifndef TEST_TOOLS_COMMAND_LINE_HPP
#define TEST_TOOLS_COMMAND_LINE_HPP

#include <vector>
#include <memory>

#include "cli/subcommand.hpp"


namespace cli
{
	class CommandLine
	{
	public:
		int execute(int argc, char** argv);
		void show_help() const;

		void add_subcommand(std::unique_ptr<Subcommand> subcommand);

	private:
		std::vector<std::unique_ptr<Subcommand>> subcommands_;

	};
}

#endif //TEST_TOOLS_COMMAND_LINE_HPP
