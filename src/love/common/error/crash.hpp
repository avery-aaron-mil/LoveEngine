#ifndef LOVE_CRASH_HPP
#define LOVE_CRASH_HPP
#include <love/common/love_common.hpp>

#include <functional>
#include <string>
#include <vector>

namespace love_engine {
    class LV_ENG_COM Crash {
        public:
            static void setCrashFunction(const std::function<void(const std::string&)>& crashFunction) noexcept;
            static void setCrashDirectory(const std::string& directory) noexcept;
            static void setCrashPath(const std::string& filePath) noexcept;
            static void setFlavorTexts(const std::vector<std::string>& flavorTexts) noexcept;

            // @throw std::logic_error If a crash is already initiated.
            [[noreturn]] static void crash(const std::string& message);
    };
}

#endif // LOVE_CRASH_HPP
