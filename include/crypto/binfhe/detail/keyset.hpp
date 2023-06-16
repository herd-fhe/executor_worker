#ifndef EXECUTOR_WORKER_BINFHE_KEYSET_HPP
#define EXECUTOR_WORKER_BINFHE_KEYSET_HPP

#include <binfhecontext-ser.h>

#include "crypto/i_keyset.hpp"


namespace crypto::binfhe::detail
{
	class KeysetImpl: public IKeyset
	{
	public:
		KeysetImpl(lbcrypto::BinFHEContext context, const lbcrypto::LWESwitchingKey& switching_key, const lbcrypto::RingGSWACCKey& refresh_key) noexcept;

		[[nodiscard]] virtual herd::common::SchemaType schema_type() const noexcept override;

		[[nodiscard]] const lbcrypto::BinFHEContext& context() const noexcept;

	private:
		lbcrypto::BinFHEContext context_;

		lbcrypto::LWESwitchingKey switching_key_;
		lbcrypto::RingGSWACCKey refresh_key_;
	};
}

#endif //EXECUTOR_WORKER_BINFHE_KEYSET_HPP
