#ifndef EXECUTOR_WORKER_CRYPTO_FACTORY_HPP
#define EXECUTOR_WORKER_CRYPTO_FACTORY_HPP

#include <set>
#include <functional>
#include <memory>
#include <filesystem>

#include "herd/common/model/worker/crypto_key_ptr.hpp"

#include "base_runtime/crypto/crypto.hpp"


namespace crypto
{
	class CryptoFactory
	{
	public:
		using FactoryMethod = std::function<std::unique_ptr<Crypto>(const std::filesystem::path&)>;

		CryptoFactory() = delete;

		static bool register_crypto(herd::common::SchemaType type, FactoryMethod method);
		static bool is_registered(herd::common::SchemaType type) noexcept;
		static std::set<herd::common::SchemaType> registered_schemas() noexcept;

		static std::unique_ptr<Crypto> create_crypto(herd::common::CryptoKeyPtr key_ptr, const herd::common::UUID& session_uuid, const std::filesystem::path& key_base_dir);

	private:
		static std::unordered_map<herd::common::SchemaType, FactoryMethod>& get_internal_map();
	};
}

#endif //EXECUTOR_WORKER_CRYPTO_FACTORY_HPP
