#ifndef LOVE_SYSTEM_INFO_HPP
#define LOVE_SYSTEM_INFO_HPP

#include "../data/string/utf8_str.hpp"

#include <stdint>

namespace love_engine {

    class Hardware_Info {
        public:
            Hardware_Info() {}
            ~Hardware_Info() {}
            
            void init() noexcept;
            const UTF8_Str& OS() noexcept { return _OS_Name; }
            
        private:
            UTF8_Str _OS_Name;
            UTF8_Str _CPU_Name;
            UTF8_Str _video_Card_Name;
            uint64_t _physical_Memory;
    };

}

#endif // LOVE_SYSTEM_INFO_HPP