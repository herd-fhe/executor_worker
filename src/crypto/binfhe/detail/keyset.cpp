#include "crypto/binfhe/detail/keyset.hpp"


namespace crypto::binfhe::detail
{
	KeysetImpl::KeysetImpl(lbcrypto::BinFHEContext context, const lbcrypto::LWESwitchingKey& switching_key, const lbcrypto::RingGSWACCKey& refresh_key) noexcept
	:	context_(std::move(context))
	{
		context_.BTKeyLoad({refresh_key, switching_key});
	}

	herd::common::SchemaType KeysetImpl::schema_type() const noexcept
	{
		return herd::common::SchemaType::BINFHE;
	}

	const lbcrypto::BinFHEContext& KeysetImpl::context() const noexcept
	{
		return context_;
	}
}
