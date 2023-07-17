#ifndef EXECUTOR_WORKER_WORKER_CONTROLLER_HPP
#define EXECUTOR_WORKER_WORKER_CONTROLLER_HPP

#include <worker.grpc.pb.h>

#include "config.hpp"


class WorkerController: public herd::proto::Worker::Service
{
public:
	explicit WorkerController(Config config);

	grpc::Status map(grpc::ServerContext* context, const herd::proto::MapTask* request, herd::proto::Empty* response) override;
	grpc::Status reduce(grpc::ServerContext* context, const herd::proto::ReduceTask* request, herd::proto::Empty* response) override;

private:
	Config config_;
};

#endif //EXECUTOR_WORKER_WORKER_CONTROLLER_HPP
