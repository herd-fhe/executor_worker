#ifndef EXECUTOR_WORKER_RUNNABLE_NODE_HPP
#define EXECUTOR_WORKER_RUNNABLE_NODE_HPP

#include "herd/common/model/circuit/nodes.hpp"

#include "crypto/crypto_value.hpp"


struct RunnableNode
{
	herd::common::node_t node;
	unsigned int parent_count;
	unsigned int completed_parents = 0;
	unsigned int child_count;
	unsigned int completed_children = 0;

	crypto::crypto_value_ptr_t value;

	RunnableNode(herd::common::node_t stored_node, unsigned int node_parent_count, unsigned int node_child_count)
	:	node(stored_node), parent_count(node_parent_count), child_count(node_child_count)
	{};
};

using runnable_node_ptr_t = std::shared_ptr<RunnableNode>;

#endif //EXECUTOR_WORKER_RUNNABLE_NODE_HPP
