#include "system_info.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <sstream>
#include <iostream> // TODO Remove

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
    
    void SystemInfo::_find_CPU() noexcept {
#ifdef _WIN32
        IEnumWbemClassObject* queryResults = _wmi_Instance.query_System_Info(L"SELECT * FROM Win32_Processor");
        do {
            IWbemClassObject* wbemClassObj = _wmi_Instance.get_Next_Query_Object(&queryResults);
            if (wbemClassObj) {
                _CPU_Processor_Count++;
                _CPU_Core_Count += _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfCores");
                _CPU_Thread_Count += _wmi_Instance.get_Object_Value_UI32(wbemClassObj, L"NumberOfLogicalProcessors");
                _CPU_Names.push_back(_wmi_Instance.get_Object_Value_String(wbemClassObj, L"Name"));
                wbemClassObj->Release();
            }
        }
        while (queryResults);
#elif defined(__unix__)
        // TODO Use /proc/cpuinfo
#else
#error "OS not supported"
#endif
        std::stringstream buffer;
        for (size_t i = 0; i < _CPU_Names.size(); i++) {
            buffer << _CPU_Names[i];
            if (i < _CPU_Names.size() - 1) buffer << "; ";
        }
        _CPU_Names_Consolidated.assign(buffer.str());
    }
    
    void SystemInfo::_find_Video_Card() noexcept {
        // TODO
    }
    
    void SystemInfo::_find_Physical_Memory() noexcept {
        // TODO
    }
    
    // TODO Find audio, monitor, and disk devices, and voltage/temp info using
    // sysconf and fscanf on hwmon (Linux), and GetSystemInfo and ManagementObjectSelector (Windows)
    // Linux: https://stackoverflow.com/questions/150355/ and https://stackoverflow.com/questions/23716135/
    // Windows: https://www.codeproject.com/articles/17973/how-to-get-hardware-information-cpu-id-mainboard-i
    
    // TODO Get free space using statvfs (Linux) and GetDiskFreeSpaceA (Windows)
    
    // TODO Get battery info using GetSystemPowerStatus (Windows) and ACPID (Linux)
    // https://stackoverflow.com/questions/27613517/querying-the-power-status-of-a-linux-machine-programmatically
#include <stdio.h> // TODO REMOVE
    std::string SystemInfo::get_Consolidated_System_Info() noexcept {
        // TODO Return all information in a fancy little string
        std::stringstream buffer;
        buffer << "OS: " << get_OS() <<
            "\nCPU Name: " << get_CPU() <<
            "\nNumber of cores: " << get_CPU_Core_Count() <<
            "\nNumber of processor threads: " << get_CPU_Thread_Count() << "\n";
        std::puts(buffer.str().c_str());
        return buffer.str();
    }
}
