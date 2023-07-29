#ifndef LOVE_SYSTEM_INFO_HPP
#define LOVE_SYSTEM_INFO_HPP
#include <love/common/love_common.hpp>

#include <string>
#include <vector>

namespace love_engine {
    class LV_ENG_COM SystemInfo {
        public:
            struct CPUInfo {
                std::string name;
                std::string description;
                std::string threads;
                std::string speed;
            };
            struct VideoCardInfo {
                std::string name;
                std::string driverVersion;
                std::string memory;
            };
            struct BaseBoardInfo {
                std::string name;
                std::string biosVendor;
                std::string biosVersion;
                std::string systemName;
            };

            static std::string getConsolidatedSystemInfo() noexcept;
            static std::string getOS() noexcept;
            static std::string getSystemName() noexcept;
            static CPUInfo getCPU() noexcept;
            static std::vector<VideoCardInfo> getVideoCards() noexcept;
            static BaseBoardInfo getBaseBoard() noexcept;
            static std::string getPhysicalMemory() noexcept;

        private:
            static void _setConsolidatedSystemInfo() noexcept;
            static void _findOS() noexcept;
            static void _findSystemName() noexcept;
            static void _findCPU() noexcept;
            static void _findVideoCards() noexcept;
            static void _findBaseBoard() noexcept;
            static void _findPhysicalMemory() noexcept;
    };

}

#endif // LOVE_SYSTEM_INFO_HPP