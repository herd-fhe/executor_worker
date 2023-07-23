#ifndef TEST_TOOLS_SUBCOMMAND_HPP
#define TEST_TOOLS_SUBCOMMAND_HPP

#include <span>
#include <string>
#include <string_view>
#include <memory>
#include <functional>

#include "cli/parameter.hpp"
#include "cli/parameter_proxy.hpp"


namespace cli
{
	class Subcommand
	{
	public:
		explicit Subcommand(std::string name, std::function<int(const ParameterProxy& proxy)> action);

		int execute(std::span<const std::string_view> arguments);
		const std::string& name();

		const std::string& description();
		void set_description(std::string_view description);

		void show_help();

		void register_parameter(std::unique_ptr<Parameter> parameter);

	private:
		struct Arguments
		{
			std::map<std::string_view, std::string_view> named;
			std::vector<std::string_view> positional;
		};

		std::string name_;
		std::string description_;

		std::vector<std::unique_ptr<Parameter>> parameters_;

		std::function<int(const ParameterProxy& proxy)> action_;

		[[nodiscard]] Arguments parse_arguments(std::span<const std::string_view> arguments) const;
	};
}

#endif //TEST_TOOLS_SUBCOMMAND_HPP
