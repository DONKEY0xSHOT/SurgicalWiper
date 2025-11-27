#pragma once
#include <filesystem>
#include <vector>

class FileProcessor {
public:

    // Recursively processes directory & XORs specific ranges in supported files
    void processDirectory(const std::filesystem::path& dirPath, uint8_t key);

private:
    void processFile(const std::filesystem::path& filePath, uint8_t key);

    // Reads file into memory, gets intervals & applies in-place XOR
    bool Overwrite(const std::filesystem::path& path, char charTOOverrideWith);
};