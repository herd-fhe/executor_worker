#include "cli/parameter.hpp"


namespace cli
{

	Parameter::Parameter(std::string name, bool is_flag)
	:	name_(std::move(name)), flag_(is_flag)
	{
	}

	const std::string& Parameter::name() const
	{
		return name_;
	}

	const std::string& Parameter::description() const
	{
		return description_;
	}

	void Parameter::set_description(const std::string& description)
	{
		description_ = description;
	}

	bool Parameter::is_flag() const
	{
		return flag_;
	}
}