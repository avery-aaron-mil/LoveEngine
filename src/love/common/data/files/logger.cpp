#include "logger.hpp"

#include "../../system/threads.hpp"

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sstream>
#include <sys/time.h>

namespace love_engine {
    void Logger::log(const Log_Status status, const std::string& message) const noexcept {
        std::thread::id id = std::this_thread::get_id();
        //auto thread = Threads::create_Thread("LogAsync", _create_and_Log_Message, status, std::cref(message), std::cref(id));
        std::thread thread(_create_and_Log_Message, status, std::cref(message), std::cref(id));
    }

    void Logger::_create_and_Log_Message(
        const Log_Status status,
        const std::string& message,
        const std::thread::id& threadId
    ) const noexcept {
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
            " [" << Threads::get_Thread_Name(threadId) << "/" <<
            LOG_TYPE_STRINGS[static_cast<int>(status)] << "]: " <<
            message << "\n"
        ;

        try {
            FileIO::append_File(_logPath.c_str(), message);
        } catch (std::runtime_error &e) {
            std::stringstream error;
            error << "FileIO::append_File() failed. Error:\n\t" << e.what();
            // TODO Crash error.str()
        }
    }
}
