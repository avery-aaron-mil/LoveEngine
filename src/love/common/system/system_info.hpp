#ifndef LOVE_DEVICE_INFO_HPP
#define LOVE_DEVICE_INFO_HPP

#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
#include "wmi_instance.hpp"
#endif

namespace love_engine {

    class SystemInfo {
        public:
            SystemInfo() {}
            ~SystemInfo() {}

            std::string get_Consolidated_System_Info() noexcept;
            const std::string& get_OS() noexcept {
                if (_OS_Name.empty()) _find_OS();
                return _OS_Name;
            }
            const std::string& get_CPU() noexcept {
                if (_CPU_Names_Consolidated.empty()) _find_CPU();
                return _CPU_Names_Consolidated;
            }
            uint32_t get_CPU_Processor_Count() noexcept {
                if (_CPU_Processor_Count <= 0) _find_CPU();
                return _CPU_Processor_Count;
            }
            uint32_t get_CPU_Core_Count() noexcept {
                if (_CPU_Core_Count <= 0) _find_CPU();
                return _CPU_Core_Count;
            }
            uint32_t get_CPU_Thread_Count() noexcept {
                if (_CPU_Thread_Count <= 0) _find_CPU();
                return _CPU_Thread_Count;
            }
            const std::string& get_Video_Card() noexcept {
                if (_video_Card_Name.empty()) _find_Video_Card();
                return _video_Card_Name;
            }
            uint64_t get_Physical_Memory() noexcept {
                if (_physical_Memory <= 0) _find_Physical_Memory();
                return _physical_Memory;
            }

        private:
#ifdef _WIN32
            WMI_Instance _wmi_Instance;
#endif 
            std::string _OS_Name;
            std::string _CPU_Names_Consolidated;
            uint32_t _CPU_Processor_Count = 0;
            std::vector<std::string> _CPU_Names;
            uint32_t _CPU_Core_Count = 0;
            uint32_t _CPU_Thread_Count = 0;
            std::string _video_Card_Name;
            uint64_t _physical_Memory = 0;

            void _find_OS() noexcept;
            void _find_CPU() noexcept;
            void _find_Video_Card() noexcept;
            void _find_Physical_Memory() noexcept;
    };

}

#endif // LOVE_DEVICE_INFO_HPP