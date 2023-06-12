#include "device_info.hpp"

#include <lzma.h>

#if defined(_WIN32)
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
    void Device_Info::_find_OS() const noexcept {
        // TODO Use a string buffer and make this neater
#if defined(_WIN32)
        OS.append("Windows");

        OSVERSIONINFOA versionInfo{};
        versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&versionInfo);

        switch (versionInfo.dwMajorVersion) {
            case 10:
                switch (versionInfo.dwMinorVersion) {
                    case 0:
                        OS.append(" 10");
                    break;
                    default:
                        OS.append(" v10 Other");
                    break;
                }
            break;
            case 6:
                switch (versionInfo.dwMinorVersion) {
                    case 3:
                        OS.append(" 8.1");
                    break;
                    case 2:
                        OS.append(" 8");
                    break;
                    case 1:
                        OS.append(" 7");
                    break;
                    case 0:
                        OS.append(" Vista");
                    break;
                    default:
                        OS.append(" v6 Other");
                    break;
                }
            break;
            case 5:
                OS.append(" XP");
            break;
            default:
                OS.append(" Other");
            break;
        }

        if (String::isAscii(versionInfo.szCSDVersion)) OS.append(versionInfo.szCSDVersion);
#elif defined(__unix__)
        struct utsname unameData;
        uname(&unameData);
        OS.append(nameData.sysname);
        OS.append(' ');
        OS.append(nameData.release);
#else
#error "OS not supported"
#endif
        System::_OS = OS.str();
    }
    
    void Device_Info::_find_CPU() const noexcept {
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
    
    void Device_Info::_find_CPU_Thread_Count() const noexcept {
        _CPU_Thread_Count = lzma_cputhreads();
        // TODO Check for errors or make custom device query
    }
    
    void Device_Info::_find_Video_Card() const noexcept {
        // TODO Find a way to get this without Vulcan
    }
    
    void Device_Info::_find_Physical_Memory() const noexcept {
        _physical_Memory = lzma_physmem();
        // TODO Check for errors or make custom device query
    }
    
    // TODO Remove underscores. they're ugly.
    
    // TODO Find audio, monitor, and disk devices, and voltage/temp info using
    // sysconf and fscanf on hwmon (Linux), and GetSystemInfo and ManagementObjectSelector (Windows)
    // Linux: https://stackoverflow.com/questions/150355/ and https://stackoverflow.com/questions/23716135/
    // Windows: https://www.codeproject.com/Articles/17973/
    
    // TODO Get free space using statvfs (Linux) and GetDiskFreeSpaceA (Windows)
    
    // TODO Get battery info using GetSystemPowerStatus (Windows) and ACPID (Linux)
    // https://stackoverflow.com/questions/27613517/querying-the-power-status-of-a-linux-machine-programmatically
}
