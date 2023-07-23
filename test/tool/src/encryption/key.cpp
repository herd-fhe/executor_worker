#include "encryption/key.hpp"

#include <binfhecontext-ser.h>


namespace
{
	std::unique_ptr<lbcrypto::BinFHEContext> generate_context(bool demo)
	{
		auto context = std::make_unique<lbcrypto::BinFHEContext>();
		context->GenerateBinFHEContext(demo? lbcrypto::TOY : lbcrypto::STD128);
		return context;
	}

	lbcrypto::LWEPrivateKey generate_private_key(lbcrypto::BinFHEContext& context)
	{
		lbcrypto::LWEPrivateKey private_key = context.KeyGen();
		return private_key;
	}

	void init_cloud_keys(lbcrypto::BinFHEContext& context, const lbcrypto::LWEPrivateKey& private_key)
	{
		context.BTKeyGen(private_key);
	}

	void serialize_context(lbcrypto::BinFHEContext& context, const std::filesystem::path& context_path)
	{
		std::ofstream context_stream(context_path, std::ios::out | std::ios::binary);

		lbcrypto::Serial::Serialize(context, context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Serialize(context.GetRefreshKey(), context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Serialize(context.GetSwitchKey(), context_stream, lbcrypto::SerType::BINARY);
	}

	void serialize_private_key(const lbcrypto::LWEPrivateKey& private_key, const std::filesystem::path& key_path)
	{
		std::ofstream key_stream(key_path, std::ios::out | std::ios::binary);

		lbcrypto::Serial::Serialize(private_key, key_stream, lbcrypto::SerType::BINARY);
	}
}

void generate_key(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, bool demo)
{
	auto context = generate_context(demo);
	auto private_key = generate_private_key(*context);
	init_cloud_keys(*context, private_key);

	serialize_context(*context, context_path);
	serialize_private_key(private_key, private_key_path);
}
