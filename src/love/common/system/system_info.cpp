#include <love/common/system/system_info.hpp>

#include <cstdint>
#include <cstring>
#include <sstream>
#include <thread>

#ifdef _WIN32
  #ifndef _WIN32_DCOM
  #define _WIN32_DCOM
  #endif
  #include <windows.h>
  #include <love/common/system/windows_registry.hpp>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <sys/utsname.h>
  #include <sys/sysctl.h>
#elif defined(__unix__)
  #include <sys/utsname.h>
  #include <sys/sysinfo.h>
#endif

namespace love_engine {
    static std::string _consolidatedSystemInfo;
    static std::string _OS;
    static std::string _systemName;
    static SystemInfo::CPUInfo _CPU;
    static std::vector<SystemInfo::VideoCardInfo> _videoCards;
    static SystemInfo::BaseBoardInfo _baseBoard;
    static std::string _physicalMemory;

    void SystemInfo::_findOS() noexcept {
#ifdef _WIN32
        const std::string version = WindowsRegistry::getHKLMValueString(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"ProductName"
        );
        if (version != "Not found") {
            std::stringstream buffer;
            buffer << version << " (" << WindowsRegistry::getHKLMValueString(
                L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                L"DisplayVersion"
            ) << ") v" << WindowsRegistry::getHKLMValueI32(
                L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                L"CurrentMajorVersionNumber"
            ) << "." << WindowsRegistry::getHKLMValueI32(
                L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                L"CurrentMinorVersionNumber"
            ) << "." << WindowsRegistry::getHKLMValueString(
                L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                L"CurrentBuildNumber"
            );
            _OS.assign(buffer.str());
        } else _OS.assign("Not found");
#elif defined(__unix__)
        std::stringstream buffer;
        struct utsname unameData;
        std::uname(&unameData);
        std::stringstream buffer(unameData.sysname);
        buffer << " ";
        buffer << unameData.release;
        _OS.assign(buffer.str());
#endif
    }

    
    void SystemInfo::_findSystemName() noexcept {
#ifdef _WIN32
        _systemName = WindowsRegistry::getHKLMValueString(
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\DataStore\\Machine\\0",
            L"szName"
        );
#elif defined(__unix__)
        // TODO
#endif
    }
    
    void SystemInfo::_findCPU() noexcept {
#ifdef _WIN32
        _CPU.name = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString"
        );
        const std::string description = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"Identifier"
        );
        if (description != "Not found") {
            std::stringstream buffer;
            buffer << description << " " << WindowsRegistry::getHKLMValueString(
                L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                L"VendorIdentifier"
            );
            _CPU.description = buffer.str();
        }
        _CPU.speed = std::to_string(WindowsRegistry::getHKLMValueI32(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"~MHz"
        )) + "MHz";
#elif defined(__APPLE__)
        // TODO Use sysctlbyname https://stackoverflow.com/questions/853798
        if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) < 0) {
            // Error
        }
#elif defined(__unix__)
        // TODO Use /proc/cpuinfo
        FILE *cpuinfo = std::fopen("/proc/cpuinfo", "rb");
        char *arg = 0;
        size_t size = 0;
        while(std::getdelim(&arg, &size, 0, cpuinfo) != -1) { // TODO Search for requested information
            std::puts(arg);
        }
        std::free(arg);
        std::fclose(cpuinfo);
#endif
        _CPU.threads = std::to_string(std::thread::hardware_concurrency());
    }
    
    void SystemInfo::_findVideoCards() noexcept {
#ifdef _WIN32
        std::vector<std::wstring> registryKeys = WindowsRegistry::getHKLMChildren(
            L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}"
        );
        for (const auto& dir : registryKeys) {
            if (dir.c_str()[0] == L'0') {
                VideoCardInfo gpu;
                gpu.name = WindowsRegistry::getHKLMValueString(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"HardwareInformation.AdapterString"
                );
                gpu.driverVersion = WindowsRegistry::getHKLMValueString(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"DriverVersion"
                );
                
                int64_t memory = WindowsRegistry::getHKLMValueI64(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"HardwareInformation.qwMemorySize"
                );
                if (memory < 0) {
                    memory = static_cast<int64_t>(
                        WindowsRegistry::getHKLMValueI32(
                            L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                            L"HardwareInformation.MemorySize"
                        )
                    );
                }
                if (memory < 0) {
                    gpu.memory = "Not found";
                } else gpu.memory = std::to_string(memory / (1024 * 1024)) + "MB";
                
                _videoCards.push_back(std::move(gpu));
            }
        }
#elif defined(__unix__)
        // TODO Use lspci -mm
#endif
    }
    
    void SystemInfo::_findBaseBoard() noexcept {
#ifdef _WIN32
        _baseBoard.name = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BaseBoardProduct"
        );
        _baseBoard.biosVendor = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BIOSVendor"
        );
        _baseBoard.biosVersion = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BIOSVersion"
        );
        _baseBoard.systemName = WindowsRegistry::getHKLMValueString(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"SystemProductName"
        );
#elif defined(__unix__)
        // TODO https://stackoverflow.com/questions/52762707 Use /sys/devices/virtual/dmi/id/board_xxx where xxx is things like name and vendor.
#endif
    }
    
    void SystemInfo::_findPhysicalMemory() noexcept {
#ifdef _WIN32
        MEMORYSTATUSEX memory;
        memory.dwLength = sizeof(memory);
        GlobalMemoryStatusEx(&memory);
        _physicalMemory = std::to_string(memory.ullTotalPhys / (1024 * 1024)) + "MB";
#elif defined(__unix__)
        struct sysinfo systemInfo;
        sysinfo(&systemInfo);
        _physicalMemory = std::to_string(systemInfo.totalram / (1024 * 1024)) + "MB"; // TODO Check if this works on Mac, and make sure it doesn't overflow
#endif
    }
    // sysconf and fscanf on hwmon (Linux)
    // Linux: https://stackoverflow.com/questions/23716135/
    
    // TODO Get free space using statvfs (Linux) and GetDiskFreeSpaceA (Windows)

    void SystemInfo::_setConsolidatedSystemInfo() noexcept {
        std::stringstream buffer;
        buffer << "OS: " << getOS();
        buffer << "\nSystem Name: " << getSystemName();
        buffer << "\nPhysical Memory: " << getPhysicalMemory();

        getCPU();
        buffer << "\nCPU:";
        buffer << "\n\tName: " << _CPU.name;
        buffer << "\n\tDescription: " << _CPU.description;
        buffer << "\n\tThreads: " << _CPU.threads;
        buffer << "\n\tMax Speed: " << _CPU.speed;

        buffer << "\nVideo Cards:";
        if (getVideoCards().empty()) {
            buffer << "\n\tNo video cards found.";
        } else {
            for (const auto& gpu : getVideoCards()) {
                buffer << "\n\t" << gpu.name;
                buffer << "\n\t\tDriver Version: " << gpu.driverVersion;
                buffer << "\n\t\tMemory: " << gpu.memory;
            }
        }

        getBaseBoard();
        buffer << "\nBase Board:";
        buffer << "\n\tName: " << _baseBoard.name;
        buffer << "\n\tBIOS Vendor: " << _baseBoard.biosVendor;
        buffer << "\n\tBIOS Version: " << _baseBoard.biosVersion;
        buffer << "\n\tSystem Product Name: " << _baseBoard.systemName;

        _consolidatedSystemInfo.assign(buffer.str());
    }

    std::string SystemInfo::getConsolidatedSystemInfo() noexcept {
        if (_consolidatedSystemInfo.empty()) _setConsolidatedSystemInfo();
        return _consolidatedSystemInfo;
    }
    std::string SystemInfo::getOS() noexcept {
        if (_OS.empty()) _findOS();
        return _OS;
    }
    std::string SystemInfo::getSystemName() noexcept {
        if (_systemName.empty()) _findSystemName();
        return _systemName;
    }
    SystemInfo::CPUInfo SystemInfo::getCPU() noexcept {
        if (_CPU.name.empty()) _findCPU();
        return _CPU;
    }
    std::vector<SystemInfo::VideoCardInfo> SystemInfo::getVideoCards() noexcept {
        if (_videoCards.empty()) _findVideoCards();
        return _videoCards;
    }
    SystemInfo::BaseBoardInfo SystemInfo::getBaseBoard() noexcept {
        if (_baseBoard.name.empty()) _findBaseBoard();
        return _baseBoard;
    }
    std::string SystemInfo::getPhysicalMemory() noexcept {
        if (_physicalMemory.empty()) _findPhysicalMemory();
        return _physicalMemory;
    }
}
