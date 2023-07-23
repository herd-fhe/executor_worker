#include "cli/parameter_proxy.hpp"


namespace cli
{
	ParameterProxy::ParameterProxy(std::string_view subcommand, const std::map<std::string_view, std::string_view>& named, const std::vector<std::string_view>& positional)
	:	subcommand_(subcommand), named_(named), positional_(positional)
	{
	}

	std::string_view ParameterProxy::subcommand() const
	{
		return subcommand_;
	}

	bool ParameterProxy::boolean(const std::string& key) const
	{
		if(const auto iter = named_.find(key); iter != std::end(named_))
		{
			return true;
		}

		return false;
	}

	std::string ParameterProxy::string(const std::string& key) const
	{
		if(const auto iter = named_.find(key); iter != std::end(named_))
		{
			return std::string(iter->second);
		}

		throw ParameterMissingError("Missing key: " + key);
	}

	std::string ParameterProxy::string(const std::string& key, std::string default_value) const noexcept
	{
		if(const auto iter = named_.find(key); iter != std::end(named_))
		{
			return std::string(iter->second);
		}
		else
		{
			return default_value;
		}
	}


	std::string ParameterProxy::positional_string(std::size_t index) const
	{
		if(index < positional_.size())
		{
			return std::string(positional_[index]);
		}

		throw ParameterMissingError("Missing positional with index: " + std::to_string(index));
	}

	std::string ParameterProxy::positional_string(std::size_t index, std::string default_value) const noexcept
	{
		if(index < positional_.size())
		{
			return std::string(positional_[index]);
		}
		else
		{
			return default_value;
		}
	}
}