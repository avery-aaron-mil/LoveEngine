#include "logger.hpp"

#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace love_engine {
    void Logger::log(const std::string& message) const noexcept {
        std::puts(message.c_str());
        std::puts("\n");
    }
    
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        std::puts(message.c_str());
        std::puts("\n");
    }

    void Logger::setConsole() {
#ifdef _WIN32
                if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE) {
                    setvbuf(stdout, NULL, _IONBF, 0);
                }
#endif
    }
}
