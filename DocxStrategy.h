#pragma once
#include "IWiperStrategy.h"
#include <string>

class DocxStrategy : public IWiperStrategy {
public:
    std::vector<WipeInterval> scan(const std::vector<uint8_t>& data) override;

private:
    const std::vector<uint8_t> ZIP_SIG = { 0x50, 0x4B, 0x03, 0x04 };
    const std::string TARGET_FILE = "word/document.xml";
};