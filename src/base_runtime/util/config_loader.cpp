#include "base_runtime/util/config_loader.hpp"

#include <charconv>

#include <spdlog/spdlog.h>

#ifdef __linux__
	#include <unistd.h>
	#include <charconv>
#else
	#error "System not supported"
#endif


namespace
{
	std::filesystem::path get_self_executable_dir_path()
	{
		static auto self_dir = []() {
			char self_path[FILENAME_MAX];
			ssize_t count = readlink("/proc/self/exe", self_path, FILENAME_MAX);
			if(count <= 0)
			{
				throw std::runtime_error("Failed to find self directory");
			}
			std::filesystem::path self_dir_path(self_path);
			self_dir_path.remove_filename();

			return self_dir_path;
		}();

		return self_dir;
	}
}


Config load_config()
{
	Config config{};

	if(const char* key_base_dir = std::getenv("KEY_BASE_DIR"))
	{
		config.key_base_dir = key_base_dir;
	}
	else
	{
		config.key_base_dir = get_self_executable_dir_path();
	}

	if(const char* storage_base_dir = std::getenv("STORAGE_BASE_DIR"))
	{
		config.storage_base_dir = storage_base_dir;
	}
	else
	{
		config.storage_base_dir = get_self_executable_dir_path();
	}

	if(const char* worker_port = std::getenv("WORKER_PORT"))
	{
		const std::string port_string = worker_port;
		auto [ptr, ec] { std::from_chars(port_string.data(), port_string.data() + port_string.size(), config.listening_port) };

		if (ec == std::errc::invalid_argument)
	 	{
			spdlog::error("Invalid string provided for port value");
			throw std::runtime_error("Invalid port");
		}
		else if (ec == std::errc::result_out_of_range)
		{
			spdlog::error("Port out of range");
			throw std::runtime_error("Invalid port");
		}
	}
	else
	{
		config.listening_port = 5001;
	}

	return config;
}
