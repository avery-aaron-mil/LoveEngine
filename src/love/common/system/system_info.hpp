#ifndef LOVE_DEVICE_INFO_HPP
#define LOVE_DEVICE_INFO_HPP

#include <string>
#include <vector>

namespace love_engine {

    class SystemInfo {
        public:
            typedef struct CPU_Info_ {
                std::string name;
                std::string description;
                std::string threads;
                std::string speed;
            } CPU_Info;
            typedef struct Video_Card_Info_ {
                std::string name;
                std::string driverVersion;
                std::string memory;
            } VideoCardInfo;
            typedef struct BaseBoardInfo_ {
                std::string name;
                std::string biosVendor;
                std::string biosVersion;
                std::string systemName;
            } BaseBoardInfo;

            static std::string get_Consolidated_System_Info();
            static std::string get_OS();
            static std::string get_System_Name();
            static CPU_Info get_CPU();
            static std::vector<VideoCardInfo> get_Video_Cards();
            static BaseBoardInfo get_Base_Board();
            static std::string get_Physical_Memory();

        private:
            static void _set_Consolidated_System_Info();
            static void _find_OS();
            static void _find_System_Name();
            static void _find_CPU();
            static void _find_Video_Cards();
            static void _find_Base_Board();
            static void _find_Physical_Memory();
    };

}

#endif // LOVE_DEVICE_INFO_HPP