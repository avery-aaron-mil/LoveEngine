#include "logger.hpp"

#include <cstdio>
#include <future>
#include <sstream>

namespace love_engine {
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        std::stringstream outputMessageBuffer;

        // TODO Get time

        switch (status) {
            case Log_Status::LOG_IGNORE: {
                // TODO Set prefix
            } break;
            case Log_Status::LOG_INFO:
            default: {
                // TODO Set prefix
            } break;
        }

        outputMessageBuffer << message << "\n";

        std::string outputMessage = outputMessageBuffer.str();
        auto task = std::async(std::launch::async, _logAsync, _filePath, outputMessage);
    }

    void Logger::_logAsync(const std::string& filePath, const std::string& message) {
        try {
            FileIO::append_File(filePath.c_str(), message);
        } catch (std::runtime_error &e) {
            // TODO Crash
        }
    }
}
