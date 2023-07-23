#include "cli/subcommand.hpp"

#include <iostream>


namespace cli
{

	Subcommand::Subcommand(std::string name, std::function<int(const ParameterProxy& proxy)> action)
	:	name_(std::move(name)), action_(std::move(action))
	{
	}

	const std::string& Subcommand::name()
	{
		return name_;
	}

	const std::string& Subcommand::description()
	{
		return description_;
	}

	void Subcommand::set_description(std::string_view description)
	{
		description_ = description;
	}

	void Subcommand::register_parameter(std::unique_ptr<Parameter> parameter)
	{
		parameters_.emplace_back(std::move(parameter));
	}

	void Subcommand::show_help()
	{
		std::cout << description_ << "\n";
		for(const auto& parameter: parameters_)
		{
			std::cout << "\t--" << parameter->name();

			if(!parameter->is_flag())
			{
				std::cout << " value";
			}

			std::cout << " - " << parameter->description() << "\n";
		}
	}

	int Subcommand::execute(std::span<const std::string_view> arguments)
	{
		const auto parsed_arguments = parse_arguments(arguments);

		try
		{
			return action_({
					name_,
					parsed_arguments.named,
					parsed_arguments.positional
			});
		}
		catch(const ParameterMissingError& error)
		{
			std::cout << error.what() << "\n\n";
			show_help();
			std::flush(std::cout);
		}

		return 1;
	}

	Subcommand::Arguments Subcommand::parse_arguments(std::span<const std::string_view> arguments) const
	{
		Subcommand::Arguments parsed_arguments{};

		auto iter = std::begin(arguments);
		while(iter != std::end(arguments))
		{
			if(iter->starts_with("--"))
			{
				const auto key = iter->substr(2);
				for(const auto& parameter: parameters_)
				{
					if(parameter->name() == key)
					{
						if(parameter->is_flag())
						{
							parsed_arguments.named.try_emplace(key, "");
						}
						else if(const auto next = std::next(iter); next != std::end(arguments))
						{
							if(!next->starts_with("--"))
							{
								parsed_arguments.named.try_emplace(key, *next);
								++iter;
							}
						}
					}
				}
			}
			else
			{
				parsed_arguments.positional.emplace_back(*iter);
			}

			++iter;
		}

		return parsed_arguments;
	}
}