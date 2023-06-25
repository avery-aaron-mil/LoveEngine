#ifndef LOVE_CRASH_HPP
#define LOVE_CRASH_HPP

#include <functional>
#include <string>
#include <vector>

namespace love_engine {
    class Crash {
        public:
            static void set_Crash_Function(std::function<void(const std::string&)> crashFunction);
            static void set_Crash_Directory(const std::string& directory);
            static void set_Crash_Path(const std::string& filePath);
            static void set_Flavor_Texts(const std::vector<std::string>& flavorTexts);
            [[noreturn]] static void crash(const std::string& message);
    };
}

#endif // LOVE_CRASH_HPP
