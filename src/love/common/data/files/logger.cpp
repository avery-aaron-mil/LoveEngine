#include "logger.hpp"

#include <stdio.h>

namespace love_engine {
    void Logger::log(const std::string& message) const noexcept {
        std::puts(message.c_str());
        std::puts("\n");
    }
    
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        std::puts(message.c_str());
        std::puts("\n");
    }
}
