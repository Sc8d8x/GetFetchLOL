#pragma once
#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H
#include <vector>
#include <string>
#include <cstdint>
#include <memory>


struct DeviceInfo {
    std::string osName;
    std::string osVersion;
    std::string architecture;
    uint64_t totalRAM; 
    uint32_t processorCount;
    std::string processorModel;
    std::string hostname;
    std::string Gpuname;
    void print() const;
    void displayWithAscii() const;
    std::vector<std::string> getInfoAsLines() const;
};

class DeviceInfoCollector {
public:
    virtual ~DeviceInfoCollector() = default;
    virtual DeviceInfo collect() = 0;

    static std::unique_ptr<DeviceInfoCollector> create();
};

void operation();
#endif 