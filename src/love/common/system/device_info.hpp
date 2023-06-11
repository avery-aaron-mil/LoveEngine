#ifndef LOVE_SYSTEM_INFO_HPP
#define LOVE_SYSTEM_INFO_HPP

#include <cstdint>
#include <string>

namespace love_engine {

    class Device_Info {
        public:
            Device_Info() {
                _find_OS();
                _find_CPU();
                _find_Video_Card();
                _find_Physical_Memory();
            }
            ~Device_Info() {}

            const std::string& get_OS() const noexcept { return _OS_Name; }
            const std::string& get_CPU() const noexcept { return _CPU_Name; }
            const std::string& get_Video_Card() const noexcept { return _video_Card_Name; }
            uint64_t get_Physical_Memory() const noexcept { return _physical_Memory; }

        private:
            std::string _OS_Name;
            std::string _CPU_Name;
            std::string _video_Card_Name;
            uint64_t _physical_Memory;

            void _find_OS() const noexcept;
            void _find_CPU() const noexcept;
            void _find_Video_Card() const noexcept;
            void _find_Physical_Memory() const noexcept;
    };

}

#endif // LOVE_SYSTEM_INFO_HPP