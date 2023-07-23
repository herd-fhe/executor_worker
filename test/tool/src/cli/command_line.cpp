#include "cli/command_line.hpp"

#include <iostream>


namespace cli
{

	int cli::CommandLine::execute(int argc, char** argv)
	{
		const std::vector<std::string_view> arguments(argv + 1, argv + argc);
		if(!arguments.empty())
		{
			const auto& subcommand_name = arguments[0];
			for(const auto& subcommand: subcommands_)
			{
				if(subcommand_name == subcommand->name())
				{
					return subcommand->execute({arguments.data()+1, arguments.size()-1});
				}
			}

			std::cout << "Subcommand " << subcommand_name << "not supported\n";
		}
		else
		{
			std::cout << "No subcommand selected\n";
		}

		show_help();
		return 1;
	}

	void CommandLine::show_help() const
	{
		std::cout << "\nAvailable commands:\n";

		for(const auto& subcommand: subcommands_)
		{
			std::cout << subcommand->name() << " - " << subcommand->description() << "\n";
		}
		std::flush(std::cout);
	}

	void CommandLine::add_subcommand(std::unique_ptr<Subcommand> subcommand)
	{
		subcommands_.emplace_back(std::move(subcommand));
	}
}