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

            typedef struct CPU_Info_ {
                std::string name;
                std::string description;
                std::string socket;
                uint32_t cores = 0;
                uint32_t threads = 0;
                uint32_t speedMax = 0;
                uint32_t l2CacheSize = 0;
                uint32_t l3CacheSize = 0;
            } CPU_Info;
            typedef struct Video_Card_Info_ {
                std::string name;
                std::string videoMode;
                std::string driverVersion;
                uint32_t memory = 0;
                uint32_t refreshRateMin = 0;
                uint32_t refreshRateMax = 0;
                uint32_t bitsPerPixel = 0;
            } Video_Card_Info;

            std::string get_Consolidated_System_Info() noexcept {
                if (_consolidated_System_Info.empty()) _set_Consolidated_System_Info();
                return _OS_Name;
            }
            const std::string& get_OS() noexcept {
                if (_OS_Name.empty()) _find_OS();
                return _OS_Name;
            }
            const std::vector<CPU_Info>& get_CPUs() noexcept {
                if (_CPUs.empty()) _find_CPUs();
                return _CPUs;
            }
            const std::vector<Video_Card_Info>& get_Video_Cards() noexcept {
                if (_video_Cards.empty()) _find_Video_Cards();
                return _video_Cards;
            }
            uint64_t get_Physical_Memory() noexcept {
                if (_physical_Memory <= 0) _find_Physical_Memory();
                return _physical_Memory;
            }

        private:
#ifdef _WIN32
            WMI_Instance _wmi_Instance;
#endif 
            std::string _consolidated_System_Info;
            std::string _OS_Name;
            std::vector<CPU_Info> _CPUs;
            std::vector<Video_Card_Info> _video_Cards;
            uint64_t _physical_Memory = 0;

            void _set_Consolidated_System_Info() noexcept;
            void _find_OS() noexcept;
            void _find_CPUs() noexcept;
            void _find_Video_Cards() noexcept;
            void _find_Physical_Memory() noexcept;
    };

}

#endif // LOVE_DEVICE_INFO_HPP