#include "system_info.hpp"

#include <cstdint>
#include <cstring>
#include <sstream>
#include <iostream> // TODO Remove

#ifdef _WIN32
  #ifndef _WIN32_DCOM
  #define _WIN32_DCOM
  #endif
  #include <Windows.h>
  #include "windows_registry.hpp"
#elif defined(__linux__)
  #include <unistd.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <stdlib.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <sys/utsname.h>
  #include <iterator>
  #include <stdlib.h>
#else
#error "OS not supported"
#endif

namespace love_engine {
    void SystemInfo::_find_OS() noexcept {
        std::stringstream buffer;
#ifdef _WIN32
        buffer << WindowsRegistry::get_HKLM_Value_String(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"ProductName"
        ) << " (" << WindowsRegistry::get_HKLM_Value_String(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"DisplayVersion"
        ) << ") v" << WindowsRegistry::get_HKLM_Value_I32(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"CurrentMajorVersionNumber"
        ) << "." << WindowsRegistry::get_HKLM_Value_I32(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"CurrentMinorVersionNumber"
        ) << "." << WindowsRegistry::get_HKLM_Value_String(
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"CurrentBuildNumber"
        );
#elif defined(__unix__)
        struct utsname unameData;
        uname(&unameData);
        std::stringstream buffer(unameData.sysname);
        buffer << " ";
        buffer << nameData.release;
#else
#error "OS not supported"
#endif
        _OS.assign(buffer.str());
    }

    
    void SystemInfo::_find_System_Name() noexcept {
#ifdef _WIN32
        _systemName = WindowsRegistry::get_HKLM_Value_String(
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Group Policy\\DataStore\\Machine\\0",
            L"szName"
        );
#elif defined(__unix__)
        // TODO
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_CPU() noexcept {
#ifdef _WIN32
        _CPU.name = WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"ProcessorNameString"
        );
        std::stringstream buffer;
        buffer << WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"Identifier"
        ) << " " << WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"VendorIdentifier"
        );
        _CPU.description = buffer.str();
        _CPU.threads = std::to_string(WindowsRegistry::get_HKLM_Children(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor"
        ).size());
        _CPU.speed = std::to_string(WindowsRegistry::get_HKLM_Value_I32(
            L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            L"~MHz"
        )) + "MHz";
#elif defined(__unix__)
        // TODO Use /proc/cpuinfo
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_Video_Cards() noexcept {
#ifdef _WIN32
        std::vector<std::wstring> registryKeys = WindowsRegistry::get_HKLM_Children(
            L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}"
        );
        for (auto dir : registryKeys) {
            if (dir.c_str()[0] == L'0') {
                VideoCardInfo gpu;
                gpu.name = WindowsRegistry::get_HKLM_Value_String(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"HardwareInformation.AdapterString"
                );
                gpu.driverVersion = WindowsRegistry::get_HKLM_Value_String(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"DriverVersion"
                );
                
                int64_t memory = WindowsRegistry::get_HKLM_Value_I64(
                    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                    L"HardwareInformation.qwMemorySize"
                );
                if (memory < 0) {
                    memory = static_cast<int64_t>(
                        WindowsRegistry::get_HKLM_Value_I32(
                            L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + dir,
                            L"HardwareInformation.MemorySize"
                        )
                    );
                }
                gpu.memory = std::to_string(memory / (1024 * 1024)) + "MB";
                
                _video_Cards.push_back(std::move(gpu));
            }
        }
#elif defined(__unix__)
        // TODO
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_Base_Board() noexcept {
#ifdef _WIN32
        _baseBoard.name = WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BaseBoardProduct"
        );
        _baseBoard.biosVendor = WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BIOSVendor"
        );
        _baseBoard.biosVersion = WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"BIOSVersion"
        );
        _baseBoard.systemName = WindowsRegistry::get_HKLM_Value_String(
            L"HARDWARE\\DESCRIPTION\\System\\BIOS",
            L"SystemProductName"
        );
#elif defined(__unix__)
        // TODO
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_Physical_Memory() noexcept {
#ifdef _WIN32
        MEMORYSTATUSEX memory;
        memory.dwLength = sizeof(memory);
        GlobalMemoryStatusEx(&memory);
        _physicalMemory = std::to_string(memory.ullTotalPhys / (1024 * 1024)) + "MB";
#elif defined(__unix__)
        // TODO
#else
#error "OS not supported"
#endif
    }
    
    // TODO Find audio, monitor, and disk devices, and voltage/temp info using
    // sysconf and fscanf on hwmon (Linux)
    // Linux: https://stackoverflow.com/questions/150355/ and https://stackoverflow.com/questions/23716135/
    
    // TODO Get free space using statvfs (Linux) and GetDiskFreeSpaceA (Windows)
    
    // TODO Get battery info using GetSystemPowerStatus (Windows) and ACPID (Linux)
    // https://stackoverflow.com/questions/27613517/querying-the-power-status-of-a-linux-machine-programmatically

    void SystemInfo::_set_Consolidated_System_Info() noexcept {
        // TODO Make a table generator
        std::stringstream buffer;
        buffer << "OS: " << get_OS();
        buffer << "\nSystem Name: " << get_System_Name();
        buffer << "\nPhysical Memory: " << get_Physical_Memory();

        get_CPU();
        buffer << "\n\nCPU:";
        buffer << "\n\tName: " << _CPU.name;
        buffer << "\n\tDescription: " << _CPU.description;
        buffer << "\n\tThreads: " << _CPU.threads;
        buffer << "\n\tMax Speed: " << _CPU.speed;

        buffer << "\n\nVideo Cards:";
        for (auto gpu : get_Video_Cards()) {
            buffer << "\n\t" << gpu.name;
            buffer << "\n\t\tDriver Version: " << gpu.driverVersion;
            buffer << "\n\t\tMemory: " << gpu.memory;
        }

        get_Base_Board();
        buffer << "\n\nBase Board:";
        buffer << "\n\tName: " << _baseBoard.name;
        buffer << "\n\tBIOS Vendor: " << _baseBoard.biosVendor;
        buffer << "\n\tBIOS Version: " << _baseBoard.biosVersion;
        buffer << "\n\tSystem Product Name: " << _baseBoard.systemName;

        std::cout << buffer.str() << std::endl;
        _consolidated_System_Info.assign(buffer.str());
    }
}
