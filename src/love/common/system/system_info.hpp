#ifndef LOVE_DEVICE_INFO_HPP
#define LOVE_DEVICE_INFO_HPP

#include <cstdint>
#include <string>

#ifdef _WIN32
#include "wmi_instance.hpp"
#endif

namespace love_engine {

    class SystemInfo {
        public:
            SystemInfo() {}
            ~SystemInfo() {}

            std::string get_Consolidated_System_Info() const noexcept;
            const std::string& get_OS() const noexcept { return _OS_Name; }
            const std::string& get_CPU() const noexcept { return _CPU_Name; }
            uint32_t get_CPU_Thread_Count() const noexcept { return _CPU_Thread_Count; }
            const std::string& get_Video_Card() const noexcept { return _video_Card_Name; }
            uint64_t get_Physical_Memory() const noexcept { return _physical_Memory; }

        private:
#ifdef _WIN32
            WMI_Instance _wmi_Instance;
#endif 
            std::string _OS_Name;
            std::string _CPU_Name;
            uint64_t _CPU_Thread_Count;
            std::string _video_Card_Name;
            uint64_t _physical_Memory;

            void _find_OS() noexcept;
            void _find_CPU() noexcept;
            void _find_CPU_Thread_Count() noexcept;
            void _find_Video_Card() noexcept;
            void _find_Physical_Memory() noexcept;
            
            class CPU_ID {
                uint32_t registers[4];

                public:
                    explicit CPU_ID(unsigned funcID, unsigned subFuncID) {
                        asm volatile(
                            "cpuid"
                            : "=a" (registers[0]), "=b" (registers[1]),
                            "=c" (registers[2]), "=d" (registers[3])
                            : "a" (funcID), "c" (subFuncID)
                        );
                        // RCX is set to zero for CPUID function 4
                    }

                    const uint32_t &RAX() const {return registers[0];}
                    const uint32_t &RBX() const {return registers[1];}
                    const uint32_t &RCX() const {return registers[2];}
                    const uint32_t &RDX() const {return registers[3];}
            };
    };

}

#endif // LOVE_DEVICE_INFO_HPP