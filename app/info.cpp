#include "device_info.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <array>
#include <memory> 
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "advapi32.lib")
#elif defined(__linux__) || defined(__unix__)
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <cstdio>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <cstdio>
#endif

using namespace std;

#ifdef _WIN32
class RegistryKey {
    HKEY key;
public:
    RegistryKey(HKEY k) : key(k) {}
    ~RegistryKey() { if (key) RegCloseKey(key); }
    HKEY get() { return key; }
};
#endif

static size_t safePadding(size_t currentLen, size_t maxLen = 45) {
    return (currentLen < maxLen) ? (maxLen - currentLen) : 0;
}

void DeviceInfo::print() const {
    cout << "OS: " << osName << " " << osVersion << endl;
    cout << "Architecture: " << architecture << endl;

    if (totalRAM > 0) {
        cout << "RAM: " << totalRAM / (1024 * 1024 * 1024) << " GB" << endl;
    }

    cout << "Processors: " << processorCount << endl;

    if (!processorModel.empty()) {
        cout << "CPU: " << processorModel << endl;
    }

    if (!Gpuname.empty() && Gpuname != "Unknown GPU") {
        cout << "GPU: " << Gpuname << endl;
    }

    cout << "Hostname: " << hostname << endl;
}

vector<string> DeviceInfo::getInfoAsLines() const {
    vector<string> lines;

    // os
    size_t osLen = 7 + osName.length() + osVersion.length();
    lines.push_back("  OS: " + osName + " " + osVersion +
        string(safePadding(osLen), ' '));

    // architecture 
    size_t archLen = 16 + architecture.length();
    lines.push_back("  Architecture: " + architecture +
        string(safePadding(archLen), ' '));

    // ram
    if (totalRAM > 0) {
        string ramStr = to_string(totalRAM / (1024 * 1024 * 1024)) + " GB";
        size_t ramLen = 7 + ramStr.length();
        lines.push_back("  RAM: " + ramStr +
            string(safePadding(ramLen), ' '));
    }

    // processors
    size_t procLen = 14 + to_string(processorCount).length();
    lines.push_back("  Processors: " + to_string(processorCount) +
        string(safePadding(procLen), ' '));

    // CPU
    if (!processorModel.empty()) {
        string cpu = processorModel;
        if (cpu.length() > 25) {
            cpu = cpu.substr(0, 22) + "...";
        }
        size_t cpuLen = 7 + cpu.length();
        lines.push_back("  CPU: " + cpu +
            string(safePadding(cpuLen), ' '));
    }

    // GPU
    if (!Gpuname.empty() && Gpuname != "Unknown GPU") {
        string gpu = Gpuname;
        if (gpu.length() > 25) {
            gpu = gpu.substr(0, 22) + "...";
        }
        size_t gpuLen = 7 + gpu.length();
        lines.push_back("  GPU: " + gpu +
            string(safePadding(gpuLen), ' '));
    }

    // hostname
    if (!hostname.empty()) {
        size_t hostLen = 12 + hostname.length();
        lines.push_back("  Hostname: " + hostname +
            string(safePadding(hostLen), ' '));
    }

    return lines;
}

void DeviceInfo::displayWithAscii() const {
    vector<string> asciiArt = {
        "=====================------------=-----------",
        "==================-----=---------------------",
        "==========-------------=---------------------",
        "==-=-------------------=---------------------",
        "----------===-------=**+===--+**+==----------",
        "------=====++++==---+**+=+=--***+==----------",
        "----==+==+**++++++==+**+=+=--+**+==----------",
        "--==++==**++==++++==++*+=+=--+**+==----------",
        "-=+++++++====+++*+==+*++++=--***+==----------",
        "=***+++=++===+++*+=++**+++=--***+==----------",
        "+##*++=**=+++=*+**=+**#*+*+--***+==----------",
        "+%#*+++#**+++=#*+#++#***++=--**++==----------",
        "-###**#%%#++=-****+#%#*+---::=-====------+**+",
        "++####%%%#+--==+#**#%#*+:+:-:++++++=-----####",
        "**##%%##**+::-.:=:=%#**++*+=-*****+=-::-*####",
        "++**%%%%*#+-.....-##***+**++=*****+=====*####",
        "+=-=+**++##-...-*##+*#*+++++=****+*++##***###",
        "+-:-------+-.:=#*+++*#*++++++**#*+*++#****###",
        "::::::-----:..-*++++##*+++***###***#*#*****##",
        "+=---::::--::.:**++**#*+*+*##**+=-*###******#",
        "##%%#*=-..:::.+#***####***#*+-:::-#######**##",
        "########*=---*####++****+=-:::::::=*+++++++**",
        "######****=*%%#%%*.-*+=-:.::-::.:=+++++++++++",
        "=+*##*****##=++*#+.--:::::---:::-+*++++++++++",
        "..-+*******=.+-++*-::+=---::::::+**++++++++++",
        "...:-=****++:*-++++:.=**=----:.=*****++******",
        "....::-##*+*=+=:=*+..:-:---=-==******++******"
    };

    vector<string> infoLines = getInfoAsLines();

    size_t maxAsciiWidth = 0;
    for (const auto& line : asciiArt) {
        if (line.length() > maxAsciiWidth) {
            maxAsciiWidth = line.length();
        }
    }

    size_t padding = 4;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n";

    size_t maxLines = max(asciiArt.size(), infoLines.size());

    for (size_t i = 0; i < maxLines; i++) {
        if (i < asciiArt.size()) {
            cout << left << setw(maxAsciiWidth) << asciiArt[i];
        }
        else {
            cout << string(maxAsciiWidth, ' ');
        }

        cout << string(padding, ' ');

        if (i < infoLines.size()) {
            cout << infoLines[i];
        }

        cout << endl;
    }

    cout << "\n";
}

std::string getHostname() {
    char buffer[256] = { 0 };

#ifdef _WIN32
    DWORD size = sizeof(buffer);
    if (GetComputerNameA(buffer, &size)) {
        buffer[sizeof(buffer) - 1] = '\0';
        return std::string(buffer);
    }
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    if (gethostname(buffer, sizeof(buffer)) == 0) {
        buffer[sizeof(buffer) - 1] = '\0';
        return std::string(buffer);
    }
#endif

    return "unknown";
}

unsigned getProcessorCount() {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#elif defined(__linux__) || defined(__APPLE__)
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 1;
#endif
}

#ifdef _WIN32

typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

class WindowsDeviceInfo : public DeviceInfoCollector {
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.hostname = getHostname();
        HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
        if (hMod) {
            RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
            if (RtlGetVersion != nullptr) {
                RTL_OSVERSIONINFOW osvi = { 0 };
                osvi.dwOSVersionInfoSize = sizeof(osvi);
                if (RtlGetVersion(&osvi) == 0) {
                    info.osVersion = std::to_string(osvi.dwMajorVersion) + "." +
                        std::to_string(osvi.dwMinorVersion);

                    if (osvi.dwMajorVersion == 11) {
                        info.osName = "Windows 11";
                    }
                    else if (osvi.dwMajorVersion == 10) {
                        info.osName = "Windows 10";
                    }
                    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
                        info.osName = "Windows 8.1";
                    }
                    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
                        info.osName = "Windows 8";
                    }
                    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
                        info.osName = "Windows 7";
                    }
                    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) {
                        info.osName = "Windows Vista";
                    }
                    else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
                        info.osName = "Windows XP 64-bit/Server 2003";
                    }
                    else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
                        info.osName = "Windows XP";
                    }
                    else {
                        info.osName = "Windows";
                    }
                }
            }
        }
        else {
            info.osName = "Windows";
        }

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        info.processorCount = sysInfo.dwNumberOfProcessors;

        if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
            info.architecture = "x64";
        }
        else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
            info.architecture = "x86";
        }
        else {
            info.architecture = "Unknown";
        }

        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);
        if (GlobalMemoryStatusEx(&memoryStatus)) {
            info.totalRAM = memoryStatus.ullTotalPhys;
        }

        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegistryKey regKey(hKey);
            char cpuName[256] = { 0 };
            DWORD size = sizeof(cpuName);
            if (RegQueryValueExA(regKey.get(), "ProcessorNameString", NULL, NULL,
                (LPBYTE)cpuName, &size) == ERROR_SUCCESS) {
                cpuName[sizeof(cpuName) - 1] = '\0';
                info.processorModel = cpuName;
            }
        }

        HKEY hkey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000",
            0, KEY_READ, &hkey) == ERROR_SUCCESS) {
            RegistryKey regKey(hkey);
            char infoGPU[256] = { 0 };
            DWORD size = sizeof(infoGPU);
            if (RegQueryValueExA(regKey.get(), "DriverDesc", NULL, NULL,
                (LPBYTE)infoGPU, &size) == ERROR_SUCCESS) {
                infoGPU[sizeof(infoGPU) - 1] = '\0';
                info.Gpuname = infoGPU;
            }
            else {
                info.Gpuname = "Unknown GPU";
            }
        }
        else {
            info.Gpuname = "Unknown GPU";
        }

        return info;
    }
};
#endif

#ifdef __linux__

class LinuxDeviceInfo : public DeviceInfoCollector {
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.hostname = getHostname();

        struct utsname sysInfo;
        if (uname(&sysInfo) == 0) {
            info.osName = sysInfo.sysname;
            info.osVersion = sysInfo.release;
            info.architecture = sysInfo.machine;
        }
        else {
            info.osName = "Linux";
        }

        struct sysinfo memInfo;
        if (sysinfo(&memInfo) == 0) {
            // Исправлено переполнение
            info.totalRAM = static_cast<uint64_t>(memInfo.totalram) * memInfo.mem_unit;
        }

        info.processorCount = getProcessorCount();

        std::ifstream cpuinfo("/proc/cpuinfo");
        if (cpuinfo.is_open()) {
            std::string line;
            while (std::getline(cpuinfo, line)) {
                if (line.find("model name") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos) {
                        info.processorModel = line.substr(pos + 2);
                        break;
                    }
                }
            }
            cpuinfo.close();
        }

        FILE* pipe = popen("lspci | grep -i 'vga\\|3d\\|display' | head -1", "r");
        if (pipe) {
            std::array<char, 256> buffer = { 0 };
            if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                std::string res(buffer.data());
                size_t pos = res.find(": ");
                if (pos != std::string::npos) {
                    std::string gpuName = res.substr(pos + 2);
                    if (!gpuName.empty() && gpuName.back() == '\n') {
                        gpuName.pop_back();
                    }
                    info.Gpuname = gpuName;
                }
            }
            pclose(pipe);
        }
        else {
            info.Gpuname = "Unknown GPU";
        }

        return info;
    }
};

#endif 

#ifdef __APPLE__

class MacOSInfo : public DeviceInfoCollector {
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.hostname = getHostname();
        info.osName = "macOS";

        char version[256] = { 0 };
        size_t len = sizeof(version);
        if (sysctlbyname("kern.osrelease", version, &len, NULL, 0) == 0) {
            info.osVersion = version;
        }
        else {
            info.osVersion = "Unknown";
        }

        char arch[256] = { 0 };
        len = sizeof(arch);
        if (sysctlbyname("hw.machine", arch, &len, NULL, 0) == 0) {
            info.architecture = arch;
        }
        else {
            info.architecture = "Unknown";
        }

        uint64_t memsize = 0;
        len = sizeof(memsize);
        if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) == 0) {
            info.totalRAM = memsize;
        }

        info.processorCount = getProcessorCount();

        char model[256] = { 0 };
        len = sizeof(model);
        if (sysctlbyname("machdep.cpu.brand_string", model, &len, NULL, 0) == 0) {
            info.processorModel = model;
        }
        else {
            info.processorModel = "Unknown";
        }

        return info;
    }
};

#endif 

class UnknownCollector : public DeviceInfoCollector {
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.osName = "Unknown";
        info.hostname = getHostname();
        return info;
    }
};

std::unique_ptr<DeviceInfoCollector> DeviceInfoCollector::create() {
#ifdef _WIN32
    return std::make_unique<WindowsDeviceInfo>();
#elif defined(__linux__)
    return std::make_unique<LinuxDeviceInfo>();
#elif defined(__APPLE__)
    return std::make_unique<MacOSInfo>();
#else
    return std::make_unique<UnknownCollector>();
#endif
}