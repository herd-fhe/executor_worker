#ifndef TEST_TOOLS_PARAMETER_HPP
#define TEST_TOOLS_PARAMETER_HPP

#include <string>


namespace cli
{
	class Parameter
	{
	public:
		explicit Parameter(std::string name, bool is_flag = false);

		[[nodiscard]] const std::string& name() const;

		[[nodiscard]] const std::string& description() const;
		void set_description(const std::string& description);

		[[nodiscard]] bool is_flag() const;

	private:
		std::string name_;
		std::string description_;

		bool flag_{false};
	};
}

#endif //TEST_TOOLS_PARAMETER_HPP
