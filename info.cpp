#include "device_info.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>

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
#include <memory>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <cstdio>
#include <memory>
#endif

using namespace std;

// функция определения ОС
void operation() {
#ifdef _WIN32
    cout << "Operating System: Windows" << endl;
#elif __linux__
    cout << "Operating System: Linux" << endl;
#elif __APPLE__
    cout << "Operating System: macOS" << endl;
#elif __unix__
    cout << "Operating System: Unix" << endl;
#elif defined(_POSIX_VERSION)
    cout << "Operating System: POSIX-compliant" << endl;
#else 
    cout << "Operating System: Unknown" << endl;
#endif
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
    lines.push_back("  OS: " + osName + " " + osVersion +
        string(35 - (7 + osName.length() + osVersion.length()), ' '));

    lines.push_back("  Architecture: " + architecture +
        string(35 - (17 + architecture.length()), ' '));

    if (totalRAM > 0) {
        string ramStr = to_string(totalRAM / (1024 * 1024 * 1024)) + " GB";
        lines.push_back("  RAM: " + ramStr +
            string(35 - (7 + ramStr.length()), ' '));
    }

    lines.push_back("  Processors: " + to_string(processorCount) +
        string(35 - (15 + to_string(processorCount).length()), ' '));

    if (!processorModel.empty()) {
        // обрезаем слишком длинные названия процессоров
        string cpu = processorModel;
        if (cpu.length() > 25) {
            cpu = cpu.substr(0, 22) + "...";
        }
        lines.push_back("  CPU: " + cpu +
            string(35 - (7 + cpu.length()), ' '));
    }

    if (!Gpuname.empty() && Gpuname != "Unknown GPU") {
        // обрезаем слишком длинные названия видеокарт
        string gpu = Gpuname;
        if (gpu.length() > 25) {
            gpu = gpu.substr(0, 22) + "...";
        }
        lines.push_back("  GPU: " + gpu +
            string(35 - (7 + gpu.length()), ' '));
    }

    if (!hostname.empty()) {
        lines.push_back("  Hostname: " + hostname +
            string(35 - (12 + hostname.length()), ' '));
    }


    return lines;
}

void DeviceInfo::displayWithAscii() const {
    // ASCII арт
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

    // получаем информацию как строки
    vector<string> infoLines = getInfoAsLines();

    // находим максимальную ширину ASCII арта
    size_t maxAsciiWidth = 0;
    for (const auto& line : asciiArt) {
        if (line.length() > maxAsciiWidth) {
            maxAsciiWidth = line.length();
        }
    }

    // отступ между колонками
    size_t padding = 4;

    // очищаем экран
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    cout << "\n";

    // выводим бок о бок
    size_t maxLines = max(asciiArt.size(), infoLines.size());

    for (size_t i = 0; i < maxLines; i++) {
        // ASCII арт слева
        if (i < asciiArt.size()) {
            cout << left << setw(maxAsciiWidth) << asciiArt[i];
        }
        else {
            cout << string(maxAsciiWidth, ' ');
        }

        // отступ
        cout << string(padding, ' ');

        // информация справа
        if (i < infoLines.size()) {
            cout << infoLines[i];
        }

        cout << endl;
    }

    cout << "\n";
}

// функция получения hostname 
std::string getHostname() {
    char buffer[256];

#ifdef _WIN32
    DWORD size = sizeof(buffer);
    if (GetComputerNameA(buffer, &size)) {
        return std::string(buffer);
    }
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    if (gethostname(buffer, sizeof(buffer)) == 0) {
        return std::string(buffer);
    }
#endif

    return "unknown";
}

// получаем количество ядер процессора 
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

// классы для разных систем
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

                    // определяем конкретное название ОС
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

        // архитектура
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

        // память
        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);
        if (GlobalMemoryStatusEx(&memoryStatus)) {
            info.totalRAM = memoryStatus.ullTotalPhys;
        }

        // название процессора
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char cpuName[256];
            DWORD size = sizeof(cpuName);
            if (RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL,
                (LPBYTE)cpuName, &size) == ERROR_SUCCESS) {
                info.processorModel = cpuName;
            }
            RegCloseKey(hKey);
        }

        // информация про видеокарту
        HKEY hkey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000",
            0, KEY_READ, &hkey) == ERROR_SUCCESS) {
            char infoGPU[256];
            DWORD size = sizeof(infoGPU);
            if (RegQueryValueExA(hkey, "DriverDesc", NULL, NULL,
                (LPBYTE)infoGPU, &size) == ERROR_SUCCESS) {
                RegCloseKey(hkey);
                info.Gpuname = infoGPU;
            }
            else {
                std::cout << "Unkown or Not GPU" << std::endl;
            }
        }
            
        return info;
    }
};
#endif

// linux
#ifdef __linux__ 

class LinuxDeviceInfo : public DeviceInfoCollector {
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.hostname = getHostname();

        // информация об ОС
        struct utsname sysInfo;
        if (uname(&sysInfo) == 0) {
            info.osName = sysInfo.sysname;
            info.osVersion = sysInfo.release;
            info.architecture = sysInfo.machine;
        }
        else {
            info.osName = "Linux";
        }

        // память 
        struct sysinfo memInfo;  
        if (sysinfo(&memInfo) == 0) {
            info.totalRAM = memInfo.totalram * memInfo.mem_unit;  
        }

        // процессор 
        info.processorCount = getProcessorCount();

        // модель процессора 
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
        //  информация про видеокарту

        FILE* pipe = popen(""lspci | grep - i 'vga\\|3d\\|display' | head - 1", "r"");

        if (pipe) {
            char infoGPU[256];
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                pclose(pipe);
                std::string res = infoGPU;

                size_t pos = res.find(": ");
                if (pos != std::string::npos) {
                    std::string info1 = res.substr(pos + 2);
                    if (!info1.empty() && info1[info.length() - 1] == '\n') {
                        info1.pop_back();
                    }
                }
                info.Gpuname = info1;
        }

        return info;
    }
};

#endif 

// macOS
#ifdef __APPLE__

class MacOSInfo : public DeviceInfoCollector {  
public:
    DeviceInfo collect() override {
        DeviceInfo info;
        info.hostname = getHostname();  
        info.osName = "macOS";  

        // версия
        char version[256];
        size_t len = sizeof(version);
        sysctlbyname("kern.osrelease", version, &len, NULL, 0);
        info.osVersion = version;

        // архитектура 
        char arch[256];
        len = sizeof(arch);
        sysctlbyname("hw.machine", arch, &len, NULL, 0);
        info.architecture = arch;

        // память
        uint64_t memsize;
        len = sizeof(memsize);
        sysctlbyname("hw.memsize", &memsize, &len, NULL, 0);
        info.totalRAM = memsize;

        // процессор 
        info.processorCount = getProcessorCount();

        // модель процессора
        char model[256];
        len = sizeof(model);
        sysctlbyname("machdep.cpu.brand_string", model, &len, NULL, 0);
        info.processorModel = model;

        return info;
    }
};

#endif 





std::unique_ptr<DeviceInfoCollector> DeviceInfoCollector::create() {
#ifdef _WIN32
    return std::make_unique<WindowsDeviceInfo>();
#elif defined(__linux__)
    return std::make_unique<LinuxDeviceInfo>();
#elif defined(__APPLE__)
    return std::make_unique<MacOSInfo>();
#else
    class UnknownCollector : public DeviceInfoCollector {  
    public:
        DeviceInfo collect() override {
            DeviceInfo info;
            info.osName = "Unknown";
            info.hostname = getHostname();
            return info;
        }
    };
    return std::make_unique<UnknownCollector>();
#endif
}