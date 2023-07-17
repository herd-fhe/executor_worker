#include "executor/tree_runner.hpp"

#include <cassert>
#include <utility>

#include <spdlog/spdlog.h>

#include "executor/exception.hpp"


namespace
{
	template<std::size_t count>
	bool has_operation(const std::array<herd::common::Operation, count>& operations, herd::common::Operation operation)
	{
		return std::ranges::find(operations, operation) != std::end(operations);
	}
}

TreeRunner::TreeRunner(std::size_t thread_count)
{
	assert(thread_count != 0);

	for(std::size_t i = 0; i < thread_count; ++i)
	{
		workers_.emplace_back(
			[this](std::stop_token stop_token)
			{
				while(true)
				{
					auto node = next_node(stop_token);
					if(!node.has_value())
					{
						return;
					}

					if(exception_present_.test())
					{
						continue;
					}

					try
					{
						evaluate(node.value());
					}
					catch (const ExecutorException& error)
					{
						TreeRunner::handle_exception(std::current_exception());
					}

					TreeRunner::mark_node_completed(node.value());
				}
			}
		);
	}
}

TreeRunner::~TreeRunner()
{
	for(auto& worker: workers_)
	{
		worker.request_stop();
	}

	for(auto& worker: workers_)
	{
		worker.join();
	}
}

void TreeRunner::set_crypto(std::shared_ptr<crypto::Crypto> crypto) noexcept
{
	std::unique_lock lock(execution_mutex_);
	crypto_ = std::move(crypto);
}

std::vector<crypto::CryptoVector> TreeRunner::execute(RunnableCircuit& circuit, std::vector<std::vector<crypto::CryptoVector>>&& input)
{
	std::unique_lock lock(execution_mutex_);
	assert(crypto_);

	input_ = std::move(input);

	for(auto out_size: circuit.output)
	{
		output_.emplace_back(out_size);
	}

	todo_job_counter_ = circuit.circuit_graph.node_size();

	{
		std::unique_lock queue_lock(queue_mutex_);

		for(const auto& node: circuit.circuit_graph.source_nodes())
		{
			queue_.push(node);
		}
	}
	consumers_cv_.notify_all();

	{
		std::unique_lock todo_lock(todo_mutex_);
		todo_cv_.wait(todo_lock, [this](){
			return todo_job_counter_ == 0;
		});
	}

	if(exception_)
	{
		std::rethrow_exception(exception_);
	}

	return std::move(output_);
}

std::optional<TreeRunner::graph_node_t> TreeRunner::next_node(const std::stop_token& stop_token)
{
	std::optional<TreeRunner::graph_node_t> next;

	{
		std::unique_lock queue_lock(queue_mutex_);

		consumers_cv_.wait(
				queue_lock,
				stop_token,
				[this]()
				{
					return !queue_.empty();
				}
		);

		if(!stop_token.stop_requested())
		{
			next = queue_.front();
			queue_.pop();
		}
	}

	return next;
}

void TreeRunner::mark_node_completed(TreeRunner::graph_node_t node) noexcept
{
	{
		std::unique_lock lock(queue_mutex_);

		for(auto children = node.children(); auto& child: children)
		{
			auto& child_counter = child.value()->completed_parents;
			++child_counter;

			//schedule next node with all dependencies resolved
			if(child_counter == child.value()->parent_count)
			{
				queue_.push(child);
			}
		}

		for(auto parents = node.parents(); auto& parent: parents)
		{
			auto& parent_counter = parent.value()->completed_parents;
			++parent_counter;

			//clear buffer of already used dependencies
			if(parent_counter == parent.value()->child_count)
			{
				static_cast<void>(parent.value()->value.release());
			}
		}

	}

	//todo: maybe wake only added number of tasks??
	consumers_cv_.notify_all();

	bool should_notify = false;
	{
		std::unique_lock todo_lock(todo_mutex_);
		if(!exception_present_.test())
		{
			--todo_job_counter_;
			if(todo_job_counter_ == 0)
			{
				should_notify = true;
			}
		}
	}
	if(should_notify)
	{
		todo_cv_.notify_all();
	}
}

void TreeRunner::evaluate_constant(const TreeRunner::graph_node_t& graph_node)
{
	if(!graph_node.parents().empty())
	{
		throw WrongArgumentsCount("Wrong arguments count for constant");
	}

	const auto& constant_node = std::get<herd::common::ConstantNode>(graph_node.value()->node);
	graph_node.value()->value = crypto_->constant(constant_node.value);
}

void TreeRunner::evaluate_input(const TreeRunner::graph_node_t& graph_node)
{
	if(!graph_node.parents().empty())
	{
		throw WrongArgumentsCount("Wrong arguments count for input");
	}

	const auto& input_node = std::get<herd::common::InputNode>(graph_node.value()->node);

	const auto tuple_index = input_node.tuple_index;
	const auto field_index = input_node.field_index;
	const auto bit_index = input_node.bit_index;

	if(input_.size() <= tuple_index)
	{
		throw OutOfRangeTupleAccess("Out of range tuple subscript for read operation");
	}

	if(input_[tuple_index].size() <= field_index)
	{
		throw OutOfRangeTupleAccess("Out of range field subscript for read operation");
	}

	if(input_[tuple_index][field_index].size() <= bit_index)
	{
		throw OutOfRangeBitAccess("Out of range bit subscript for read operation");
	}

	graph_node.value()->value = crypto_->copy(*input_[input_node.tuple_index][input_node.field_index][input_node.bit_index]);
}

void TreeRunner::evaluate_operation(const TreeRunner::graph_node_t& graph_node)
{
	const auto& operation_node = std::get<herd::common::OperationNode>(graph_node.value()->node);

	if(has_operation(herd::common::UNARY_OPERATIONS, operation_node.type))
	{
		auto parents = graph_node.parents();
		if(parents.size() != 1)
		{
			throw WrongArgumentsCount("Wrong arguments count for unary operations");
		}

		auto& input = *parents[0].value()->value;

		graph_node.value()->value = crypto_->unary_op(operation_node.type, input);
	}
	else if(has_operation(herd::common::BINARY_OPERATIONS, operation_node.type))
	{
		auto parents = graph_node.parents();
		if(parents.size() != 2)
		{
			throw WrongArgumentsCount("Wrong arguments count for binary operations");
		}

		auto& input_a = *parents[0].value()->value;
		auto& input_b = *parents[1].value()->value;

		graph_node.value()->value = crypto_->binary_op(operation_node.type, input_a, input_b);
	}
	else if(has_operation(herd::common::TERNARY_OPERATIONS, operation_node.type))
	{
		auto parents = graph_node.parents();
		if(parents.size() != 3)
		{
			throw WrongArgumentsCount("Wrong arguments count for ternary operations");
		}

		auto& input_a = *parents[0].value()->value;
		auto& input_b = *parents[1].value()->value;
		auto& input_c = *parents[2].value()->value;

		graph_node.value()->value = crypto_->ternary_op(operation_node.type, input_a, input_b, input_c);
	}
}

void TreeRunner::evaluate_output(const TreeRunner::graph_node_t& graph_node)
{
	if(graph_node.parents().size() != 1)
	{
		throw WrongArgumentsCount("Wrong arguments count for output");
	}

	const auto& output_node = std::get<herd::common::OutputNode>(graph_node.value()->node);

	const auto tuple_index = output_node.field_index;
	const auto bit_index = output_node.bit_index;

	if(output_.size() <= tuple_index)
	{
		throw OutOfRangeTupleAccess("Out of range tuple subscript for write operation");
	}

	if(output_[tuple_index].size() <= bit_index)
	{
		throw OutOfRangeBitAccess("Out of range bit subscript for write operation");
	}

	output_[tuple_index][bit_index] = crypto_->copy(*graph_node.parents()[0].value()->value);
}

void TreeRunner::evaluate(const TreeRunner::graph_node_t& graph_node)
{
	const auto& node = graph_node.value()->node;

	if(std::holds_alternative<herd::common::ConstantNode>(node))
	{
		spdlog::trace("Evaluating constant node");
		evaluate_constant(graph_node);
	}
	else if(std::holds_alternative<herd::common::InputNode>(node))
	{
		spdlog::trace("Evaluating input node");
		evaluate_input(graph_node);
	}
	else if(std::holds_alternative<herd::common::OperationNode>(node))
	{
		spdlog::trace("Evaluating operation node");
		evaluate_operation(graph_node);
	}
	else if(std::holds_alternative<herd::common::OutputNode>(node))
	{
		spdlog::trace("Evaluating output node");
		evaluate_output(graph_node);
	}
	else
	{
		assert(false && "Not supported node type");
	}
}

void TreeRunner::handle_exception(std::exception_ptr exception) noexcept
{
	std::unique_lock lock(exception_mutex_);

	if(!exception_)
	{
		{
			std::unique_lock todo_lock(todo_mutex_);
			exception_present_.test_and_set();
			exception_ = std::move(exception);
			todo_job_counter_ = 0;
		}
		todo_cv_.notify_all();
	}
}
