#include "logger.hpp"

#include <cstdio>
#include <future>

namespace love_engine {
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        std::puts(message.c_str());
        std::puts("\n");
        std::string prefix;
        switch (status) {
            case Log_Status::LOG_IGNORE: {
                // TODO Set prefix
            } break;
            case Log_Status::LOG_INFO:
            default: {
                // TODO Set prefix
            } break;
        }
        auto task = std::async(std::launch::async, FileIO::append_File, _filePath.c_str(), message + "\n");
    }
}
