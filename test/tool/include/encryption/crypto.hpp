#ifndef TEST_TOOLS_CRYPTO_HPP
#define TEST_TOOLS_CRYPTO_HPP

#include <filesystem>
#include <vector>


void encrypt(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, const std::filesystem::path& out_path, const std::vector<std::vector<bool>>& data);
void decrypt(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, const std::filesystem::path& in_path, std::size_t row_width, std::size_t row_count);

#endif //TEST_TOOLS_CRYPTO_HPP
