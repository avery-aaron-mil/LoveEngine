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
                cpu.cores = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfCores");
                cpu.threads = _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfLogicalProcessors");
                cpu.name = _wmi_Instance.get_Object_Value_String(wbemClassObj, L"Name");
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
    
    void SystemInfo::_find_Video_Card() noexcept {
        // TODO
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
        buffer << get_OS() << "\n";
        for (auto cpu : get_CPUs()) {
            buffer << cpu.name;
            buffer << "\n\tCores: " << cpu.cores;
            buffer << "\n\tThreads: " << cpu.threads;
        }
        std::puts(buffer.str().c_str());
        _consolidated_System_Info.assign(buffer.str());
    }
}
