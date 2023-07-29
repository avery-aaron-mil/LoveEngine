#ifndef LOVE_STACKTRACE_HPP
#define LOVE_STACKTRACE_HPP
#include <love/common/love_common.hpp>

#include <string>

namespace love_engine {
    class LV_ENG_COM StackTrace {
        public:
            static inline std::string getStacktrace() noexcept { return getStacktrace(0, 32); }
            static std::string getStacktrace(const size_t skips, const size_t depth) noexcept;
            static std::string appendStacktrace(const std::string& message) noexcept;
            static std::string appendStacktrace(std::stringstream& buffer) noexcept;
    };
}

#endif // LOVE_STACKTRACE_HPP
