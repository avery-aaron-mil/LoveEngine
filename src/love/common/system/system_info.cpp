#include "system_info.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <sstream>
#include <iostream> // TODO Remove
#include <stdio.h> // TODO REMOVE

#if defined(_WIN32)
  #ifndef _WIN32_DCOM
  #define _WIN32_DCOM
  #endif
  #include <comdef.h>
  #include <Wbemidl.h>
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
#if defined(_WIN32)
        IEnumWbemClassObject* queryResults = _wmi_Instance.query_System_Info(L"SELECT * FROM Win32_OperatingSystem");
        IWbemClassObject* wbemClassObj = _wmi_Instance.get_Next_Query_Object(&queryResults);
        std::stringstream buffer;
        buffer << _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Caption")
            << " v" << _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Version");
        wbemClassObj->Release();
        if (queryResults) queryResults->Release();
#elif defined(__unix__)
        struct utsname unameData;
        uname(&unameData);
        std::stringstream buffer(unameData.sysname);
        buffer << " ";
        buffer << nameData.release;
#else
#error "OS not supported"
#endif
        _OS_Name.assign(buffer.str());
    }
    
    void SystemInfo::_find_CPUs() noexcept {
#ifdef _WIN32
        IEnumWbemClassObject* queryResults = _wmi_Instance.query_System_Info(L"SELECT * FROM Win32_Processor");
        do {
            IWbemClassObject* wbemClassObj = _wmi_Instance.get_Next_Query_Object(&queryResults);
            if (wbemClassObj) {
                CPU_Info cpu;
                cpu.name = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Name");
                cpu.description = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Description");
                cpu.socket = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"SocketDesignation");
                cpu.cores = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfCores");
                cpu.threads = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfLogicalProcessors");
                cpu.speedMax = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"MaxClockSpeed");
                cpu.l2CacheSize = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"L2CacheSize");
                cpu.l3CacheSize = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"L3CacheSize");
                _CPUs.push_back(std::move(cpu));
                wbemClassObj->Release();
            }
        }
        while (queryResults);
#elif defined(__unix__)
        // TODO Use /proc/cpuinfo
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_Video_Cards() noexcept {
#ifdef _WIN32
        IEnumWbemClassObject* queryResults = _wmi_Instance.query_System_Info(L"SELECT * FROM Win32_VideoController");
        do {
            IWbemClassObject* wbemClassObj = _wmi_Instance.get_Next_Query_Object(&queryResults);
            if (wbemClassObj) {
                Video_Card_Info gpu;
                gpu.name = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Name");
                gpu.driverVersion = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"DriverVersion");
                gpu.videoMode = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"VideoModeDescription");
                gpu.memory = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"AdapterRAM");
                // TODO Use registry instead: https://stackoverflow.com/questions/68274009/wmi-win32-videocontroller-ram-4gb-limit
                gpu.refreshRateMin = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"MinRefreshRate");
                gpu.refreshRateMax = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"MaxRefreshRate");
                gpu.bitsPerPixel = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"CurrentBitsPerPixel");
                _video_Cards.push_back(std::move(gpu));
                wbemClassObj->Release();
            }
        }
        while (queryResults);
#elif defined(__unix__)
        // TODO
#else
#error "OS not supported"
#endif
    }
    
    void SystemInfo::_find_Physical_Memory() noexcept {
        // TODO
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
        buffer << "OS:\n\t" << get_OS();
        buffer << "\n\nCPUs:";
        for (auto cpu : get_CPUs()) {
            buffer << "\n\t" << cpu.name;
            buffer << "\n\t\tDescription: " << cpu.description;
            buffer << "\n\t\tSocket: " << cpu.socket;
            buffer << "\n\t\tCores: " << cpu.cores;
            buffer << "\n\t\tThreads: " << cpu.threads;
            buffer << "\n\t\tMax Speed: " << cpu.speedMax << "MHz";
            buffer << "\n\t\tLevel 2 Cache Size: " << cpu.l2CacheSize << "KB";
            buffer << "\n\t\tLevel 3 Cache Size: " << cpu.l3CacheSize << "KB";
        }
        buffer << "\n\nVideo Cards:";
        for (auto gpu : get_Video_Cards()) {
            buffer << "\n\t" << gpu.name;
            buffer << "\n\t\tDriver Version: " << gpu.driverVersion;
            buffer << "\n\t\tVideo Mode Description: " << gpu.videoMode;
            buffer << "\n\t\tMemory: " << gpu.memory << "B";
            buffer << "\n\t\tMinimum Refresh Rate: " << gpu.refreshRateMin << "Hz";
            buffer << "\n\t\tMaximum Refresh Rate: " << gpu.refreshRateMax << "Hz";
            buffer << "\n\t\tBits per Pixel: " << gpu.bitsPerPixel;
        }
        std::puts(buffer.str().c_str());
        _consolidated_System_Info.assign(buffer.str());
    }
}
