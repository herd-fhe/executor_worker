#ifndef EXECUTOR_WORKER_I_KEYSET_HPP
#define EXECUTOR_WORKER_I_KEYSET_HPP

#include "herd/common/model/schema_type.hpp"


namespace crypto
{
	class IKeyset
	{
	public:
		virtual ~IKeyset() = default;

		[[nodiscard]] virtual herd::common::SchemaType schema_type() const noexcept = 0;
	};
}

#endif //EXECUTOR_WORKER_I_KEYSET_HPP
