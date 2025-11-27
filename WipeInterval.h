#pragma once
#include <cstddef>

// Represents a specific byte range in a file to be processed
struct WipeInterval {
    size_t offset;
    size_t length;
};