#ifndef EXECUTOR_WORKER_TREE_RUNNER_HPP
#define EXECUTOR_WORKER_TREE_RUNNER_HPP

#include <thread>
#include <memory>
#include <queue>
#include <latch>
#include <optional>
#include <condition_variable>

#include "crypto/crypto.hpp"
#include "crypto/crypto_vector.hpp"
#include "executor/runnable_circuit.hpp"


class TreeRunner
{
public:
	explicit TreeRunner(std::size_t thread_count=std::jthread::hardware_concurrency());
	~TreeRunner();

	void set_crypto(std::shared_ptr<crypto::Crypto> crypto) noexcept;
	std::vector<crypto::CryptoVector> execute(RunnableCircuit& runnable_circuit, std::vector<crypto::CryptoVector>&& input);

private:
	using graph_node_t = decltype(RunnableCircuit::circuit_graph)::Node<false>;

	std::mutex exception_mutex_;
	std::atomic_flag exception_present_;
	std::exception_ptr exception_ = nullptr;

	std::mutex execution_mutex_;

	std::vector<std::jthread> workers_;

	std::mutex queue_mutex_;
	std::condition_variable_any consumers_cv_;

	std::mutex todo_mutex_;
	std::condition_variable todo_cv_;
	std::size_t todo_job_counter_;

	std::vector<crypto::CryptoVector> input_;
	std::vector<crypto::CryptoVector> output_;

	std::queue<graph_node_t> queue_;

	std::shared_ptr<crypto::Crypto> crypto_;

	std::optional<graph_node_t> next_node(const std::stop_token& stop_token);
	void handle_exception(std::exception_ptr exception) noexcept;
	void mark_node_completed(graph_node_t node) noexcept;

	void evaluate_constant(const TreeRunner::graph_node_t& graph_node);
	void evaluate_input(const TreeRunner::graph_node_t& graph_node);
	void evaluate_operation(const TreeRunner::graph_node_t& graph_node);
	void evaluate_output(const TreeRunner::graph_node_t& graph_node);
	void evaluate(const graph_node_t& graph_node);
};

#endif //EXECUTOR_WORKER_TREE_RUNNER_HPP
