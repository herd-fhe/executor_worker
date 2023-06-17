#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

#include "util/config_loader.hpp"
#include "controller/worker_controller.hpp"


int main()
{
	spdlog::cfg::load_env_levels();

	Config config = load_config();
	WorkerController worker_controller{config};

	grpc::ServerBuilder builder;

	const std::string address = std::string("0.0.0.0") + ":" + std::to_string(config.listening_port);
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());

	builder.RegisterService(&worker_controller);
	spdlog::debug("Controller registered");


	auto server = builder.BuildAndStart();
	spdlog::info("Server listening on address: {}", address);
	server->Wait();

	return 0;
}