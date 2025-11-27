#include "DocxStrategy.h"
#include <cstring>

std::vector<WipeInterval> DocxStrategy::scan(const std::vector<uint8_t>& data) {
    std::vector<WipeInterval> intervals;

    // Safety check for minimum ZIP header size
    if (data.size() < 30) return intervals;

    // Scan for local file header signature
    for (size_t i = 0; i < data.size() - 30; ++i) {
        if (data[i] == ZIP_SIG[0] && data[i + 1] == ZIP_SIG[1] &&
            data[i + 2] == ZIP_SIG[2] && data[i + 3] == ZIP_SIG[3]) {

            // Parse variable lengths
            uint16_t nameLen = data[i + 26] | (data[i + 27] << 8);
            uint16_t extraLen = data[i + 28] | (data[i + 29] << 8);

            // Verify filename
            if (i + 30 + nameLen > data.size()) continue;
            std::string filename(reinterpret_cast<const char*>(&data[i + 30]), nameLen);

            if (filename == TARGET_FILE) {

                // Target 1: The Filename (Anti-Carving)
                intervals.push_back({ i + 30, (size_t)nameLen });

                // Target 2: The Deflate Stream Start (Anti-Decompression)
                size_t payloadStart = i + 30 + nameLen + extraLen;

                // Wipe first 50 bytes (or remaining size if smaller)
                size_t wipeSize = 50;
                if (payloadStart + wipeSize <= data.size()) {
                    intervals.push_back({ payloadStart, wipeSize });
                }

                break;
            }
        }
    }
    return intervals;
}