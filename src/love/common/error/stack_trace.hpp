#ifndef LOVE_STACKTRACE_HPP
#define LOVE_STACKTRACE_HPP

#include <sstream>
#include <stacktrace>
#include <string>

namespace love_engine {
    class StackTrace {
        public:
            static inline std::string getStacktrace() noexcept {
                return
#ifdef _GLIBCXX_HAS_STACKTRACE
                // NOTE: Waiting for MSYS2 to implement libstdc++_libbacktrace.a, or GCC to implement <stacktrace> fully
                std::to_string(std::stacktrace::current());
#else
                "G++ has not yet fully implemented stack traces defined in C++23. :)";
#endif
            }
            static std::string appendStacktrace(const std::string& message) noexcept;
            static std::string appendStacktrace(std::stringstream& buffer) noexcept;
    };
}

#endif // LOVE_STACKTRACE_HPP
