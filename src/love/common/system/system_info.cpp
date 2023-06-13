#include "system_info.hpp"

#include <sstream>

#if defined(_WIN32)
  #include "wmi_instance.hpp"
  #include <windows.h>
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
    void SystemInfo::_find_OS() const noexcept {
#if defined(_WIN32)
        std::stringstream buffer("Windows");

        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        buffer << " 10";
                    break;
                    default:
                        buffer << " v10 Other";
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        buffer << " 8.1";
                    break;
                    case 2:
                        buffer << " 8";
                    break;
                    case 1:
                        buffer << " 7";
                    break;
                    case 0:
                        buffer << " Vista";
                    break;
                    default:
                        buffer << " v6 Other";
                    break;
                }
            break;
            case 5:
                buffer << " XP";
            break;
            default:
                buffer << " Other";
            break;
        }

        if (String::isAscii(versionInfo.szCSDVersion)) buffer << versionInfo.szCSDVersion);
#elif defined(__unix__)
        struct utsname unameData;
        uname(&unameData);
        std::stringstream buffer(unameData.sysname);
        buffer << " ";
        buffer << nameData.release;
#else
#error "OS not supported"
#endif
        _OS_Name = buffer.str();
    }
    
    void SystemInfo::_find_CPU() const noexcept {
        char buffer[sizeof(uint32_t) * (3 /* 8.2-8.4 */) * (4 /* RAX-RDX */) + 1];
        // TODO Find a C++ way to do this that doesn't involve a for loop
    
        for(uint32_t i = 0x80000002; i <= 0x80000004; i++) {
            CPU_ID cpu_ID(i, 0);
            buffer << String::asAscii(reinterpret_cast<const char*>(&cpu_ID.RAX()), 4);
            buffer << String::asAscii(reinterpret_cast<const char*>(&cpu_ID.RBX()), 4);
            buffer << String::asAscii(reinterpret_cast<const char*>(&cpu_ID.RCX()), 4);
            buffer << String::asAscii(reinterpret_cast<const char*>(&cpu_ID.RDX()), 4);
        }
        
        _cpu_Name = std::string(buffer);
    }
    
    void SystemInfo::_find_CPU_Thread_Count() const noexcept {
        // TODO
    }
    
    void SystemInfo::_find_Video_Card() const noexcept {
        // TODO
    }
    
    void SystemInfo::_find_Physical_Memory() const noexcept {
        // TODO
    }
    
    // TODO Find audio, monitor, and disk devices, and voltage/temp info using
    // sysconf and fscanf on hwmon (Linux), and GetSystemInfo and ManagementObjectSelector (Windows)
    // Linux: https://stackoverflow.com/questions/150355/ and https://stackoverflow.com/questions/23716135/
    // Windows: https://www.codeproject.com/articles/17973/how-to-get-hardware-information-cpu-id-mainboard-i
    
    // TODO Get free space using statvfs (Linux) and GetDiskFreeSpaceA (Windows)
    
    // TODO Get battery info using GetSystemPowerStatus (Windows) and ACPID (Linux)
    // https://stackoverflow.com/questions/27613517/querying-the-power-status-of-a-linux-machine-programmatically

    [[nodiscard]] std::string get_Consolidated_System_Info() const noexcept {
        // TODO Return all information in a fancy little string
        std::string info;
        return std::move(info);
    }
}
