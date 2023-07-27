#include <love/common/data/files/logger.hpp>

#include <love/common/error/crash.hpp>
#include <love/common/system/thread.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sstream>
#include <sys/time.h>

namespace love_engine {
    std::string Logger::_generateLogMessage(const LogStatus status, const std::string& message) noexcept {
        // Get time
        struct timeval tv;
        if (gettimeofday(&tv, nullptr)) {
            std::stringstream error;
            error << "gettimeofday() failed. Error: " << std::strerror(errno);
            Crash::crash(error.str());
        }
        time_t time = static_cast<time_t>(tv.tv_sec);
        std::tm* now = std::localtime(&time);
        if (now == nullptr) {
            std::stringstream error;
            error << "localtime() failed. Error: " << std::strerror(errno);
            Crash::crash(error.str());
        }

        // Set log message
        // [YYMMDD_HH::MM:SS+UUUUUU] [THREAD/TYPE]: MESSAGE
        char timeBuffer[sizeof("[YYMMDD_HH:MM:SS+UUUUUU]")];
        std::snprintf(timeBuffer, sizeof(timeBuffer), "[%02d%02d%02d/%02d:%02d:%02d+%06ld]",
            (now->tm_year - 100), (now->tm_mon + 1), now->tm_mday,
            now->tm_hour, now->tm_min, now->tm_sec, tv.tv_usec
        );
        std::stringstream outputMessageBuffer;
        outputMessageBuffer <<
            timeBuffer <<
            " [" << Thread::getThreadName(std::this_thread::get_id()) << "/" <<
            LOG_TYPE_STRINGS[static_cast<int>(status)] << "]: " <<
            message << "\n"
        ;
        return outputMessageBuffer.str();
    }

    void Logger::log(const LogStatus status, const std::string& message) const noexcept {
        std::string outputMessage = _generateLogMessage(status, message);

        std::puts(outputMessage.c_str());
        if (!_logPath.empty()) {
            try {
                FileIO::appendFile(_logPath.c_str(), outputMessage);
            } catch (std::exception& e) {
                std::stringstream error;
                error << "FileIO::appendFile() failed. Error:\n\t" << e.what();
                Crash::crash(error.str());
            }
        }
    }
}
