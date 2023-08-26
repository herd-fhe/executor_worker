#ifndef EXECUTOR_WORKER_EXECUTOR_EXCEPTION_HPP
#define EXECUTOR_WORKER_EXECUTOR_EXCEPTION_HPP

#include <stdexcept>


struct ExecutorException: public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

struct MalformedTree: public ExecutorException
{
	using ExecutorException::ExecutorException;
};

struct WrongArgumentsCount: public ExecutorException
{
	using ExecutorException::ExecutorException;
};

struct OutOfRangeTupleAccess: public ExecutorException
{
	using ExecutorException::ExecutorException;
};

struct OutOfRangeBitAccess: public ExecutorException
{
	using ExecutorException::ExecutorException;
};

#endif //EXECUTOR_WORKER_EXECUTOR_EXCEPTION_HPP
