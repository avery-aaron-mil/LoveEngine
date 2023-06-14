#include "system_info.hpp"

#include <sstream>

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
        IWbemClassObject* wbemClassObj = _wmi_Instance.get_Next_Query_Object(queryResults);
        std::stringstream buffer;
        buffer << _wmi_Instance.get_Object_Value(wbemClassObj, L"Caption")
            << " v" << _wmi_Instance.get_Object_Value(wbemClassObj, L"Version");
        wbemClassObj->Release();
        queryResults->Release();
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
        std::stringstream buffer;
    
        for(uint32_t i = 0x80000002; i <= 0x80000004; i++) {
            CPU_ID cpu_ID(i, 0);
            buffer << reinterpret_cast<const char*>(&cpu_ID.RAX());
            buffer << reinterpret_cast<const char*>(&cpu_ID.RBX());
            buffer << reinterpret_cast<const char*>(&cpu_ID.RCX());
            buffer << reinterpret_cast<const char*>(&cpu_ID.RDX());
        }

        _CPU_Name.assign(buffer.str());
    }
    
    void SystemInfo::_find_CPU_Thread_Count() noexcept {
        // TODO
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

    std::string SystemInfo::get_Consolidated_System_Info() const noexcept {
        // TODO Return all information in a fancy little string
        std::string info;
        return info;
    }
}
