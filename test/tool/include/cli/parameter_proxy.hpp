#ifndef TEST_TOOLS_PARAMETER_PROXY_HPP
#define TEST_TOOLS_PARAMETER_PROXY_HPP

#include <charconv>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>


namespace cli
{
	namespace detail
	{
		template<typename T>
		T parse_num(std::string_view num_str)
		{
			T value{};

			[[maybe_unused]] auto [ptr, ec] = std::from_chars(std::cbegin(num_str), std::cend(num_str), value);

			assert(ec == std::errc());

			return value;
		}
	}

	struct ParameterMissingError: public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	class ParameterProxy
	{
	public:
		ParameterProxy(
				std::string_view subcommand,
				const std::map<std::string_view, std::string_view>& named,
				const std::vector<std::string_view>& positional
		);

		[[nodiscard]] std::string_view subcommand() const;

		[[nodiscard]] bool boolean(const std::string& key) const;

		std::string string(const std::string& key) const;
		[[nodiscard]] std::string string(const std::string& key, std::string default_value) const noexcept;

		template<typename T>
		requires std::integral<T> || std::floating_point<T>
		T num(const std::string& key) const;

		template<typename T>
		requires std::integral<T> || std::floating_point<T>
		[[nodiscard]] T num(const std::string& key, T default_value) const noexcept;

		std::string positional_string(std::size_t index) const;
		[[nodiscard]] std::string positional_string(std::size_t index, std::string default_value) const noexcept;

		template<typename T>
		requires std::integral<T> || std::floating_point<T>
		T positional_num(std::size_t index) const;

		template<typename T>
		requires std::integral<T> || std::floating_point<T>
		[[nodiscard]] T positional_num(std::size_t index, T default_value) const noexcept;

	private:
		std::string_view subcommand_;
		const std::map<std::string_view, std::string_view>& named_;
		const std::vector<std::string_view>& positional_;
	};

	template<typename T>
	requires std::integral<T> || std::floating_point<T>
	T ParameterProxy::num(const std::string& key) const
	{
		if(const auto iter = named_.find(key); iter != std::end(named_))
		{
			return detail::parse_num<T>(iter->second);
		}

		throw ParameterMissingError("Missing key: " + key);
	}

	template<typename T>
	requires std::integral<T> || std::floating_point<T>
	T ParameterProxy::num(const std::string& key, T default_value) const noexcept
	{
		if(const auto iter = named_.find(key); iter != std::end(named_))
		{
			return detail::parse_num<T>(iter->second);
		}
		else
		{
			return default_value;
		}
	}

	template<typename T>
	requires std::integral<T> || std::floating_point<T>
	T ParameterProxy::positional_num(std::size_t index) const
	{
		if(index < positional_.size())
		{
			return detail::parse_num<T>(positional_[index]);
		}

		throw ParameterMissingError("Missing positional with index: " + std::to_string(index));
	}

	template<typename T>
	requires std::integral<T> || std::floating_point<T>
	T ParameterProxy::positional_num(std::size_t index, T default_value) const noexcept
	{
		if(index < positional_.size())
		{
			return detail::parse_num<T>(positional_[index]);
		}
		else
		{
			return default_value;
		}
	}
}

#endif //TEST_TOOLS_PARAMETER_PROXY_HPP
