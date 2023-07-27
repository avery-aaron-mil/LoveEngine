#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

#include <love/common/data/files/file_io.hpp>

#include <string>
#include <thread>

namespace love_engine {

    enum class LogStatus {
        IGNORED,
        STATUS,
        UPDATE,
        MESSAGE,
        VERBOSE,
        INFO,
        WARNING,
        ERROR_,
        FATAL,
    };

   class Logger {
        static constexpr const char* LOG_TYPE_STRINGS[] = {
            "IGNORED",
            "STATUS",
            "UPDATE",
            "MESSAGE",
            "VERBOSE",
            "INFO",
            "WARNING",
            "ERROR",
            "FATAL",
        };
        
        public:
            Logger(const std::string& filePath) : _logPath(filePath) {}
            Logger(const std::string& filePath, const bool clearFile) : _logPath(filePath) { if (clearFile) clear(); }
            ~Logger() = default;
           
            inline void log(const std::string& message) const noexcept { log(LogStatus::INFO, message); }
            virtual void log(const LogStatus status, const std::string& message) const noexcept;

            void setLogPath(const std::string& filePath) noexcept { _logPath.assign(filePath); }
            void clear() { FileIO::clearFile(_logPath.c_str()); }

        private:
            static std::string _generateLogMessage(const LogStatus status, const std::string& message) noexcept;

            std::string _logPath;
   };

}

#endif // LOVE_LOGGER_HPP
