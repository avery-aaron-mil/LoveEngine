#ifndef LOVE_CRASH_HPP
#define LOVE_CRASH_HPP

#include <string>

namespace love_engine {
    class Crash {
        public:
            static void set_Crash_Directory(const std::string& directory);
            static void set_Crash_Path(const std::string& filePath);
            [[noreturn]] static void crash(const std::string& message);
    };
}

#endif // LOVE_CRASH_HPP
