#include "mapper/circuit.hpp"

#include "herd/mapper/executor.hpp"


namespace mapper
{
	RunnableCircuit to_model(const herd::proto::Circuit& circuit_proto)
	{
		RunnableCircuit circuit{};

		circuit.input.insert(std::end(circuit.input), std::begin(circuit_proto.input()), std::end(circuit_proto.input()));
		circuit.output.insert(std::end(circuit.output), std::begin(circuit_proto.output()), std::end(circuit_proto.output()));

		auto& graph = circuit.circuit_graph;

		for(const auto& node: circuit_proto.nodes())
		{
			auto node_model = herd::mapper::to_model(node);
			auto runnable_node = std::make_shared<RunnableNode>(node_model, 0, 0);
			graph.insert(std::move(runnable_node));
		}

		for(const auto& edge: circuit_proto.edges())
		{
			const auto start = edge.start();
			const auto end = edge.end();

			const auto begin = std::begin(graph);

			const auto start_node_iter = begin + start;
			const auto end_node_iter = begin + end;

			++(*end_node_iter).value()->parent_count;
			++(*start_node_iter).value()->child_count;
		 	graph.add_edge(start_node_iter, end_node_iter);
		}

		return circuit;
	}
}