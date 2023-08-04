#include "base_runtime/executor/runnable_circuit.hpp"


void RunnableCircuit::reset_circuit() noexcept
{
	for(auto node: circuit_graph)
	{
		node.value()->completed_parents = 0;
	}
}
