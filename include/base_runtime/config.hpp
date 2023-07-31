#ifndef EXECUTOR_WORKER_CONFIG_HPP
#define EXECUTOR_WORKER_CONFIG_HPP

#include <filesystem>


struct Config
{
	unsigned int listening_port;

	std::filesystem::path key_base_dir;
	std::filesystem::path storage_base_dir;
};

#endif //EXECUTOR_WORKER_CONFIG_HPP
