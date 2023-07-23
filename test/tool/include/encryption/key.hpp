#ifndef TEST_TOOLS_KEY_HPP
#define TEST_TOOLS_KEY_HPP

#include <filesystem>


void generate_key(const std::filesystem::path& context_path, const std::filesystem::path& private_key_path, bool demo);

#endif //TEST_TOOLS_KEY_HPP
