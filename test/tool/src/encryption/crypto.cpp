#include "encryption/crypto.hpp"

#include <binfhecontext-ser.h>


namespace
{
	using namespace lbcrypto;

	std::unique_ptr<lbcrypto::BinFHEContext> load_context(const std::filesystem::path& context_path)
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

	lbcrypto::LWEPrivateKey load_private_key(const std::filesystem::path& private_key_path)
	{
		lbcrypto::LWEPrivateKey private_key;

		std::ifstream key_stream(private_key_path, std::ios::binary | std::ios::in);
		lbcrypto::Serial::Deserialize(private_key, key_stream, lbcrypto::SerType::BINARY);

		return private_key;
	}

	std::vector<lbcrypto::LWECiphertext> read_row(std::istream& stream, std::size_t row_width)
	{
		uint32_t row_raw_size = 0;
		stream.read(reinterpret_cast<char*>(&row_raw_size), sizeof(uint32_t));

		const auto start = stream.tellg();

		std::vector<lbcrypto::LWECiphertext> row_bits;
		row_bits.reserve(row_width);

		for(std::size_t i = 0; i < row_width; ++i)
		{
			lbcrypto::LWECiphertext ciphertext;
			lbcrypto::Serial::Deserialize(ciphertext, stream, lbcrypto::SerType::BINARY);

			row_bits.emplace_back(ciphertext);
		}

		if (const auto stop = stream.tellg(); stop - start != row_raw_size)
		{
			throw std::runtime_error("Invalid crypto stream");
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
}

void encrypt(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, const std::filesystem::path& out_path, const std::vector<std::vector<bool>>& data)
{
	const auto context = load_context(context_path);
	const auto private_key = load_private_key(private_key_path);

	std::ofstream data_stream(out_path, std::ios::out | std::ios::binary);

	for(const auto& row: data)
	{
		std::stringstream buffer;
		const auto start = buffer.tellp();

		for(const auto bit: row)
		{
			lbcrypto::LWECiphertext ciphertext = context->Encrypt(private_key, bit ? 1 : 0);
			lbcrypto::Serial::Serialize(ciphertext, buffer, lbcrypto::SerType::BINARY);
		}

		const auto end = buffer.tellp();
		auto size = static_cast<uint32_t>(end-start);

		data_stream.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		data_stream << buffer.rdbuf();
	}
}

void decrypt(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, const std::filesystem::path& in_path, std::size_t row_width, std::size_t row_count)
{
	const auto context = load_context(context_path);
	const auto private_key = load_private_key(private_key_path);

	std::ifstream data_stream(in_path, std::ios::binary | std::ios::in);

	for(std::size_t i = 0; i < row_count; ++i)
	{
		auto row = read_row(data_stream, row_width);
		auto decrypted_row = decrypt_row(row, *context, private_key);

		for(std::size_t j = 0; j < row_width; ++j)
		{
			std::cout << decrypted_row[j];
		}

		std::cout << "\n";
	}
}
