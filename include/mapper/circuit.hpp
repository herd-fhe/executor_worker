#ifndef EXECUTOR_WORKER_CIRCUIT_HPP
#define EXECUTOR_WORKER_CIRCUIT_HPP

#include "circuit.pb.h"

#include "executor/runnable_circuit.hpp"


namespace mapper
{
	RunnableCircuit to_model(const herd::proto::Circuit& circuit_proto);
}

#endif //EXECUTOR_WORKER_CIRCUIT_HPP
