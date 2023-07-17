#include "mapper/circuit.hpp"

#include "herd/mapper/executor.hpp"
#include "herd/mapper/storage.hpp"

#include "herd/common/native_type_mapping.hpp"


namespace mapper
{
	RunnableCircuit to_model(const herd::proto::Circuit& circuit_proto)
	{
		RunnableCircuit circuit{};

		circuit.inputs.resize(static_cast<unsigned long>(circuit_proto.inputs().size()));
		for(int i = 0; i < circuit_proto.inputs().size(); ++i)
		{
			circuit.inputs[static_cast<std::size_t>(i)].reserve(static_cast<std::size_t>(circuit_proto.inputs(i).fields().size()));
			for(int j = 0; j < circuit_proto.inputs(i).fields().size(); ++j)
			{
				const auto data_type = herd::mapper::to_model(circuit_proto.inputs(i).fields(j));
				circuit.inputs[static_cast<std::size_t>(i)].emplace_back(herd::common::data_type_to_bit_width(data_type));
			}
		}

		circuit.output.reserve(static_cast<unsigned long>(circuit_proto.output().size()));
		for(int i = 0; i < circuit_proto.output().size(); ++i)
		{
			const auto data_type = herd::mapper::to_model(circuit_proto.output(i).data_type());
			circuit.output.emplace_back(herd::common::data_type_to_bit_width(data_type));
		}

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