#pragma once
#include "IWiperStrategy.h"
#include <string>

class PdfStrategy : public IWiperStrategy {
public:
    std::vector<WipeInterval> scan(const std::vector<uint8_t>& data) override;

private:
    // Helper to find the enclosing dictionary << ... >> around a found keyword
    WipeInterval findEnclosingDictionary(const std::vector<uint8_t>& data, size_t keywordPos);
};