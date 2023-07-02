#ifndef LOVE_CRASH_HPP
#define LOVE_CRASH_HPP

#include <functional>
#include <string>
#include <vector>

namespace love_engine {
    class Crash {
        public:
            static void set_Crash_Function(const std::function<void(const std::string&)>& crashFunction) noexcept;
            static void set_Crash_Directory(const std::string& directory) noexcept;
            static void set_Crash_Path(const std::string& filePath) noexcept;
            static void set_Flavor_Texts(const std::vector<std::string>& flavorTexts) noexcept;
            // @throw std::logic_error If a crash is already initiated.
            [[noreturn]] static void crash(const std::string& message);
    };
}

#endif // LOVE_CRASH_HPP
