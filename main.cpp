#include "device_info.h"
#include <iostream>

int main() {
    try {
        auto collector = DeviceInfoCollector::create();
        DeviceInfo info = collector->collect();
        info.displayWithAscii();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}