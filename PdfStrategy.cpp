#include "PdfStrategy.h"
#include <algorithm>
#include <string_view>

std::vector<WipeInterval> PdfStrategy::scan(const std::vector<uint8_t>& data) {
    std::vector<WipeInterval> intervals;
    std::string_view dataView(reinterpret_cast<const char*>(data.data()), data.size());

    // Critical PDF Targets
    const char* targets[] = { "/Type /Catalog", "/Type/Catalog", "/Type /Pages", "/Type/Pages" };

    for (const char* target : targets) {
        size_t pos = dataView.find(target);
        while (pos != std::string_view::npos) {
            WipeInterval range = findEnclosingDictionary(data, pos);
            if (range.length > 0) {
                intervals.push_back(range);
            }
            pos = dataView.find(target, pos + 1);
        }
    }
    return intervals;
}

WipeInterval PdfStrategy::findEnclosingDictionary(const std::vector<uint8_t>& data, size_t keywordPos) {

    // Scan backwards for '<<'
    size_t start = keywordPos;
    bool foundStart = false;
    while (start > 0) {
        if (data[start] == '<' && data[start - 1] == '<') {

            // Move to first '<'
            start--;
            foundStart = true;
            break;
        }
        start--;
    }

    // Scan forwards for '>>'
    size_t end = keywordPos;
    bool foundEnd = false;
    while (end < data.size() - 1) {
        if (data[end] == '>' && data[end + 1] == '>') {

            // Move past '>>'
            end += 2; 
            foundEnd = true;
            break;
        }
        end++;
    }

    if (foundStart && foundEnd) {

        // Overwriting the entire dictionary body
        return { start, end - start };
    }
    return { 0, 0 };
}