#ifndef EXECUTOR_WORKER_RUNNABLE_CIRCUIT_HPP
#define EXECUTOR_WORKER_RUNNABLE_CIRCUIT_HPP

#include <memory>

#include "herd/common/dag.hpp"

#include "executor/runnable_node.hpp"


struct RunnableCircuit
{
	std::vector<unsigned int> input;
	std::vector<unsigned int> output;

	herd::common::DAG<runnable_node_ptr_t> circuit_graph;

	void reset_circuit() noexcept;
};

#endif //EXECUTOR_WORKER_RUNNABLE_CIRCUIT_HPP
