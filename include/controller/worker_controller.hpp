#ifndef EXECUTOR_WORKER_WORKER_CONTROLLER_HPP
#define EXECUTOR_WORKER_WORKER_CONTROLLER_HPP

#include <worker.grpc.pb.h>

#include "config.hpp"


class WorkerController: public herd::proto::Worker::Service
{
public:
	WorkerController(Config config);

	grpc::Status run(::grpc::ServerContext* context, const ::herd::proto::Task* request, ::herd::proto::Empty* response) override;

private:
	Config config_;
};

#endif //EXECUTOR_WORKER_WORKER_CONTROLLER_HPP
