#pragma once
#include "WipeInterval.h"
#include <vector>
#include <cstdint>

// Interface for file-format specific analysis
class IWiperStrategy {
public:
    virtual ~IWiperStrategy() = default;

    // Scans the provided file buffer and identifies critical structural bytes to wipe
    virtual std::vector<WipeInterval> scan(const std::vector<uint8_t>& data) = 0;
};