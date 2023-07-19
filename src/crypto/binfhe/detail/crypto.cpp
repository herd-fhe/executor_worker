#include "crypto/binfhe/detail/crypto.hpp"

#include "herd/common/model/exception.hpp"

#include "crypto/binfhe/detail/crypto_value.hpp"
#include "crypto/exception.hpp"

#include <cassert>


namespace crypto::binfhe::detail
{
	namespace
	{
		KeysetImpl& validate_keyset(const std::unique_ptr<IKeyset>& keyset)
		{
			assert(keyset->schema_type() == herd::common::SchemaType::BINFHE);
			return dynamic_cast<KeysetImpl&>(*keyset);
		}
	}

	crypto_value_ptr_t CryptoImpl::load_from_stream(std::istream& stream)
	{
		try
		{
			lbcrypto::LWECiphertext ciphertext;
			lbcrypto::Serial::Deserialize(ciphertext, stream, lbcrypto::SerType::BINARY);

			return std::make_unique<CryptoValueImpl>(std::move(ciphertext));
		}
		catch (const cereal::Exception& exception)
		{
			throw herd::common::IOReadError(exception.what());
		}
	}


	void CryptoImpl::store_to_stream(std::ostream& stream, const CryptoValue& value)
	{
		try
		{
			const auto& in = dynamic_cast<const CryptoValueImpl&>(value);

			lbcrypto::Serial::Serialize(in.value(), stream, lbcrypto::SerType::BINARY);
		}
		catch (const cereal::Exception& exception)
		{
			throw herd::common::IOWriteError(exception.what());
		}
	}

	crypto_value_ptr_t CryptoImpl::create_empty_value()
	{
		return constant(false);
	}

	CryptoImpl::CryptoImpl(std::unique_ptr<IKeyset> keyset)
	: 	keyset_holder_(std::move(keyset)), keyset_(validate_keyset(keyset_holder_))
	{
	}

	crypto_value_ptr_t CryptoImpl::ternary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& input_b, const CryptoValue& input_c)
	{
		static_cast<void>(input_a);
		static_cast<void>(input_b);
		static_cast<void>(input_c);

		switch(operation)
		{
			case herd::common::Operation::MUX:
				throw OperationNotSupported("Operation not supported for BinFHE");
			default:
				assert(false && "Wrong argument count for operation");
				return nullptr;
		}
	}

	crypto_value_ptr_t CryptoImpl::binary_op(herd::common::Operation operation, const CryptoValue& input_a, const CryptoValue& input_b)
	{
		const auto& in_a = dynamic_cast<const CryptoValueImpl&>(input_a);
		const auto& in_b = dynamic_cast<const CryptoValueImpl&>(input_b);

		lbcrypto::BINGATE gate;

		switch(operation)
		{
			case herd::common::Operation::AND:
				gate = lbcrypto::AND;
				break;
			case herd::common::Operation::OR:
				gate = lbcrypto::OR;
				break;
			case herd::common::Operation::XOR:
				gate = lbcrypto::XOR;
				break;
			case herd::common::Operation::NAND:
				gate = lbcrypto::NAND;
				break;
			default:
				assert(false && "Wrong argument count for operation");
				return nullptr;
		}

		if(in_a.value() != in_b.value())
		{
			return std::make_unique<CryptoValueImpl>(keyset_.context().EvalBinGate(gate, in_a.value(), in_b.value()));
		}
		else
		{
			auto temp_value = keyset_.context().Bootstrap(in_a.value());
			return std::make_unique<CryptoValueImpl>(keyset_.context().EvalBinGate(gate, temp_value, in_b.value()));
		}
	}
	crypto_value_ptr_t CryptoImpl::unary_op(herd::common::Operation operation, const CryptoValue& input)
	{
		const auto& in = dynamic_cast<const CryptoValueImpl&>(input);

		switch(operation)
		{
			case herd::common::Operation::NOT:
				return std::make_unique<CryptoValueImpl>(keyset_.context().EvalNOT(in.value()));
			default:
				assert(false && "Wrong argument count for operation");
				return nullptr;
		}
	}

	crypto_value_ptr_t CryptoImpl::constant(bool value)
	{
		return std::make_unique<CryptoValueImpl>(keyset_.context().EvalConstant(value));
	}

	crypto_value_ptr_t CryptoImpl::copy(const CryptoValue& input)
	{
		const auto& in = dynamic_cast<const CryptoValueImpl&>(input);

		auto copy = in.value();

		// it's probably better without bootstraping
//		return std::make_unique<CryptoValueImpl>(keyset_.context().Bootstrap(in.value()));
		return std::make_unique<CryptoValueImpl>(std::move(copy));
	}
}
