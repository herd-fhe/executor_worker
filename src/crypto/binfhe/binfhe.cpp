#include "crypto/binfhe/binfhe.hpp"

#include <fstream>

#include <binfhecontext-ser.h>

#include "herd/common/model/exception.hpp"

#include "crypto/binfhe/detail/keyset.hpp"
#include "crypto/binfhe/detail/crypto.hpp"


namespace crypto::binfhe
{
	std::unique_ptr<IKeyset> load_binfhe_keyset(const std::filesystem::path& key_path)
	{
		std::ifstream key_file(key_path, std::ios_base::binary | std::ios_base::in);
		if(key_file.fail())
		{
			throw herd::common::FileNotExistError("Could not access key file at path: " + key_path.string());
		}

		lbcrypto::BinFHEContext context{};
		lbcrypto::LWESwitchingKey switching_key;
		lbcrypto::RingGSWACCKey refresh_key;

		try
		{
			lbcrypto::Serial::Deserialize(context, key_file, lbcrypto::SerType::BINARY);
			lbcrypto::Serial::Deserialize(refresh_key, key_file, lbcrypto::SerType::BINARY);
			lbcrypto::Serial::Deserialize(switching_key, key_file, lbcrypto::SerType::BINARY);
		}
		catch(const cereal::Exception& error)
		{
			throw herd::common::IOReadError(error.what());
		}

		return std::make_unique<detail::KeysetImpl>(std::move(context), std::move(switching_key), std::move(refresh_key));
	}

	std::unique_ptr<Crypto> create_binfhe_crypto(std::unique_ptr<IKeyset> keyset)
	{
		return std::make_unique<detail::CryptoImpl>(std::move(keyset));
	}
}