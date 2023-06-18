#ifndef LOVE_DEVICE_INFO_HPP
#define LOVE_DEVICE_INFO_HPP

#include <string>
#include <vector>

namespace love_engine {

    class SystemInfo {
        public:
            SystemInfo() {}
            ~SystemInfo() {}

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

            std::string get_Consolidated_System_Info() noexcept {
                if (_consolidated_System_Info.empty()) _set_Consolidated_System_Info();
                return _OS;
            }
            const std::string& get_OS() noexcept {
                if (_OS.empty()) _find_OS();
                return _OS;
            }
            const std::string& get_System_Name() noexcept {
                if (_systemName.empty()) _find_System_Name();
                return _systemName;
            }
            const CPU_Info& get_CPU() noexcept {
                if (_CPU.name.empty()) _find_CPU();
                return _CPU;
            }
            const std::vector<VideoCardInfo>& get_Video_Cards() noexcept {
                if (_video_Cards.empty()) _find_Video_Cards();
                return _video_Cards;
            }
            const BaseBoardInfo& get_Base_Board() noexcept {
                if (_baseBoard.name.empty()) _find_Base_Board();
                return _baseBoard;
            }
            const std::string& get_Physical_Memory() noexcept {
                if (_physicalMemory.empty()) _find_Physical_Memory();
                return _physicalMemory;
            }

        private:
            std::string _consolidated_System_Info;
            std::string _OS;
            std::string _systemName;
            CPU_Info _CPU;
            std::vector<VideoCardInfo> _video_Cards;
            BaseBoardInfo _baseBoard;
            std::string _physicalMemory;

            void _set_Consolidated_System_Info() noexcept;
            void _find_OS() noexcept;
            void _find_System_Name() noexcept;
            void _find_CPU() noexcept;
            void _find_Video_Cards() noexcept;
            void _find_Base_Board() noexcept;
            void _find_Physical_Memory() noexcept;
    };

}

#endif // LOVE_DEVICE_INFO_HPP