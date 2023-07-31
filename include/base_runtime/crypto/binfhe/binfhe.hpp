#ifndef EXECUTOR_WORKER_BINFHE_HPP
#define EXECUTOR_WORKER_BINFHE_HPP

#include <filesystem>
#include <memory>

#include "herd/common/model/worker/crypto_key_ptr.hpp"
#include "herd/common/uuid.hpp"

#include "base_runtime/crypto/crypto.hpp"
#include "base_runtime/crypto/i_keyset.hpp"


namespace crypto::binfhe
{
	std::unique_ptr<IKeyset> load_binfhe_keyset(const std::filesystem::path& key_path);
	std::unique_ptr<Crypto> create_binfhe_crypto(std::unique_ptr<IKeyset> keyset);
}

#endif //EXECUTOR_WORKER_BINFHE_HPP
