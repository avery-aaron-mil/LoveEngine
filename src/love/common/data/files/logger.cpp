#include "logger.hpp"

#include "../../system/thread.hpp"

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sstream>
#include <sys/time.h>

namespace love_engine {
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        Thread asyncLogThread(
            "ASYNC_LOG",
            _create_and_Log_Message,
            _logPath, status, message, std::this_thread::get_id()
        );
    }

    void Logger::_create_and_Log_Message(
        const std::string logPath,
        const Log_Status status,
        const std::string message,
        const std::thread::id threadId
    ) {
        // Get time
        struct timeval tv;
        if (gettimeofday(&tv, nullptr)) {
            std::stringstream error;
            error << "gettimeofday() failed. Error: " << std::strerror(errno);
            // TODO Crash error.str()
        }
        time_t time = static_cast<time_t>(tv.tv_sec);
        std::tm* now = std::localtime(&time);
        if (now == nullptr) {
            std::stringstream error;
            error << "localtime() failed. Error: " << std::strerror(errno);
            // TODO Crash error.str()
        }

        // Set log message
        // [HH::MM:SS+UUUUUU] [THREAD/TYPE]: MESSAGE
        char timeBuffer[sizeof("[HH:MM:SS+UUUUUU]")];
        std::snprintf(timeBuffer, sizeof(timeBuffer), "[%02d:%02d:%02d+%06ld]",
            now->tm_hour, now->tm_min, now->tm_sec, tv.tv_usec
        );
        std::stringstream outputMessageBuffer;
        outputMessageBuffer <<
            timeBuffer <<
            " [" << Thread::get_Thread_Name(threadId) << "/" <<
            LOG_TYPE_STRINGS[static_cast<int>(status)] << "]: " <<
            message << "\n"
        ;

        std::string outputMessage = outputMessageBuffer.str();
        std::puts(outputMessage.c_str());
        try {
            FileIO::append_File(logPath.c_str(), outputMessage);
        } catch (std::runtime_error &e) {
            std::stringstream error;
            error << "FileIO::append_File() failed. Error:\n\t" << e.what();
            // TODO Crash error.str()
        }
    }
}
