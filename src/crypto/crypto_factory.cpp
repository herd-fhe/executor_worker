#include "crypto/crypto_factory.hpp"

#include <cassert>

#include "crypto/binfhe/binfhe.hpp"


namespace crypto
{
	//implementations
	namespace
	{
		[[maybe_unused]] const bool binfhe_registered = CryptoFactory::register_crypto(
			herd::common::SchemaType::BINFHE,
			[](const std::filesystem::path& key_path)
			{
				auto keyset = binfhe::load_binfhe_keyset(key_path);
				return binfhe::create_binfhe_crypto(std::move(keyset));
			}
		);
	}

	// end of implementations

	bool CryptoFactory::register_crypto(herd::common::SchemaType type, CryptoFactory::FactoryMethod method)
	{
		auto& implementations = get_internal_map();
		assert(!implementations.contains(type) && "Schema already registered");

		implementations[type] = std::move(method);
		return true;
	}

	bool CryptoFactory::is_registered(herd::common::SchemaType type) noexcept
	{
		const auto& implementations = get_internal_map();
		return implementations.contains(type);
	}

	std::set<herd::common::SchemaType> CryptoFactory::registered_schemas() noexcept
	{
		std::set<herd::common::SchemaType> schemas;
		for(const auto& implementations = get_internal_map(); const auto& [key, val]: implementations)
		{
			schemas.insert(key);
		}

		return schemas;
	}

	std::unique_ptr<Crypto> CryptoFactory::create_crypto(herd::common::CryptoKeyPtr key_ptr, const herd::common::UUID& session_uuid, const std::filesystem::path& key_base_dir)
	{
		const auto schema_type = key_ptr.schema_type;

		assert(is_registered(schema_type) && "Schema type not registered");
		const auto& implementations = get_internal_map();

		const auto concrete_factory = implementations.find(schema_type);
		assert(concrete_factory != std::end(implementations));

		const auto schema_type_name = std::to_string(static_cast<std::underlying_type_t<herd::common::SchemaType>>(schema_type)) + ".key";
		const auto key_path = key_base_dir / session_uuid.as_string() / schema_type_name;

		return concrete_factory->second(key_path);
	}

	std::unordered_map<herd::common::SchemaType, CryptoFactory::FactoryMethod>& CryptoFactory::get_internal_map()
	{
		static std::unordered_map<herd::common::SchemaType, FactoryMethod> implementations_;
		return implementations_;
	}
}