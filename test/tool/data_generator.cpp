#include <iostream>

#include <binfhecontext-ser.h>
#include <filesystem>
#include <sstream>

std::unique_ptr<lbcrypto::BinFHEContext> generate_context()
{
	auto context = std::make_unique<lbcrypto::BinFHEContext>();
	context->GenerateBinFHEContext(lbcrypto::STD128);
	return context;
}

lbcrypto::LWEPrivateKey generate_private_key(lbcrypto::BinFHEContext& context)
{
	lbcrypto::LWEPrivateKey private_key = context.KeyGen();
	return private_key;
}

void generate_cloud_keys(lbcrypto::BinFHEContext& context, const lbcrypto::LWEPrivateKey& private_key)
{
	context.BTKeyGen(private_key);
}

void store_context(lbcrypto::BinFHEContext& context, const std::filesystem::path& context_path)
{
		std::ofstream context_stream(context_path, std::ios::out | std::ios::binary);

		lbcrypto::Serial::Serialize(context, context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Serialize(context.GetRefreshKey(), context_stream, lbcrypto::SerType::BINARY);
		lbcrypto::Serial::Serialize(context.GetSwitchKey(), context_stream, lbcrypto::SerType::BINARY);
}

void store_private_key(const lbcrypto::LWEPrivateKey& private_key, const std::filesystem::path& key_path)
{
		std::ofstream key_stream(key_path, std::ios::out | std::ios::binary);

		lbcrypto::Serial::Serialize(private_key, key_stream, lbcrypto::SerType::BINARY);
}

void generate_row(std::ofstream& stream, lbcrypto::BinFHEContext& context, const lbcrypto::LWEPrivateKey& private_key, std::size_t row_size, std::size_t index)
{
		const bool reverse = index >= row_size;

		std::stringstream buffer;
		const auto start = buffer.tellp();

		std::size_t i = 0;
		for(; i < index % row_size; ++i)
		{
			lbcrypto::LWECiphertext ciphertext = context.Encrypt(private_key, reverse ? 1 : 0);
			lbcrypto::Serial::Serialize(ciphertext, buffer, lbcrypto::SerType::BINARY);
		}
		for(; i < row_size; ++i)
		{
			lbcrypto::LWECiphertext ciphertext = context.Encrypt(private_key, reverse ? 0 : 1);
			lbcrypto::Serial::Serialize(ciphertext, buffer, lbcrypto::SerType::BINARY);
		}

		const auto end = buffer.tellp();
		auto size = static_cast<uint32_t>(end - start);

		stream.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		stream << buffer.rdbuf();
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: data_generator row_bit_size" << std::endl;
		return 1;
	}

	const auto row_size = static_cast<std::size_t>(atoi(argv[1]));

	auto context = generate_context();
	auto private_key = generate_private_key(*context);
	generate_cloud_keys(*context, private_key);

	store_context(*context, "key.pub");
	store_private_key(private_key, "key");

	std::ofstream dataframe_stream("data", std::ios::out | std::ios::binary);
	for(std::size_t i = 0; i < 2*row_size; ++i)
	{
		generate_row(dataframe_stream, *context, private_key, row_size, i);
	}
	dataframe_stream.flush();
}