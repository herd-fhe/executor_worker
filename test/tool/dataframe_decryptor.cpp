#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

#include <binfhecontext-ser.h>


using namespace lbcrypto;

std::unique_ptr<BinFHEContext> load_context(const std::filesystem::path& context_path)
{
	auto context = std::make_unique<BinFHEContext>();

	{
		std::ifstream context_stream(context_path, std::ios::binary | std::ios::in);

		lbcrypto::LWESwitchingKey switching_key;
		lbcrypto::RingGSWACCKey refresh_key;

		lbcrypto::Serial::Deserialize(*context, context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Deserialize(refresh_key, context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Deserialize(switching_key, context_stream, lbcrypto::SerType::BINARY);

		context->BTKeyLoad({refresh_key, switching_key});
	}
	return context;
}

lbcrypto::LWEPrivateKey load_private_key(const std::filesystem::path& key_path)
{
	lbcrypto::LWEPrivateKey private_key;

	std::ifstream key_stream(key_path, std::ios::binary | std::ios::in);

	lbcrypto::Serial::Deserialize(private_key, key_stream, lbcrypto::SerType::BINARY);

	return private_key;
}

std::vector<lbcrypto::LWECiphertext> read_row(std::istream& stream, std::size_t bit_size)
{
	uint32_t row_raw_size = 0;
	stream.read(reinterpret_cast<char*>(&row_raw_size), sizeof(uint32_t));

	const auto start = stream.tellg();

	std::vector<lbcrypto::LWECiphertext> row_bits;
	row_bits.reserve(bit_size);

	for(std::size_t i = 0; i < bit_size; ++i)
	{
		lbcrypto::LWECiphertext ciphertext;
		lbcrypto::Serial::Deserialize(ciphertext, stream, lbcrypto::SerType::BINARY);

		row_bits.emplace_back(ciphertext);
	}

	const auto stop = stream.tellg();
	if (stop - start != row_raw_size)
	{
		exit(1);
	}

	return row_bits;
}

std::vector<bool> decrypt_row(const std::vector<lbcrypto::LWECiphertext>& row_bits, lbcrypto::BinFHEContext& context, const lbcrypto::LWEPrivateKey& private_key)
{
	std::vector<bool> decrypted_bits;
	decrypted_bits.reserve(row_bits.size());

	for(auto& bit: row_bits)
	{
		LWEPlaintext result;
		context.Decrypt(private_key, bit, &result);

		decrypted_bits.emplace_back(static_cast<bool>(result));
	}

	return decrypted_bits;
}

int main(int argc, char** argv)
{
	if(argc != 6)
	{
		std::cout << "Usage: table_decryptor context_file key_file data_frame row_bit_size row_count" << std::endl;
		return 1;
	}

	const std::filesystem::path context_path = argv[1];
	const std::filesystem::path key_path = argv[2];
	const std::filesystem::path dataframe_path = argv[3];
	const auto bit_size = static_cast<std::size_t>(atoi(argv[4]));
	const auto row_count = static_cast<std::size_t>(atoi(argv[5]));

	const auto context = load_context(context_path);
	const auto private_key = load_private_key(key_path);

	std::ifstream dataframe_stream(dataframe_path, std::ios::binary | std::ios::in);
	for(std::size_t i = 0; i < row_count; ++i)
	{
		auto row = read_row(dataframe_stream, bit_size);
		auto decrypted_row = decrypt_row(row, *context, private_key);

		for(std::size_t j = 0; j < bit_size; ++j)
		{
			std::cout << decrypted_row[j];
		}

		std::cout << "\n";
	}

	std::flush(std::cout);

	return 0;
}