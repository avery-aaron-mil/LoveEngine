#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

#include "file_io.hpp"

#include <string>
#include <thread>

namespace love_engine {

    enum class Log_Status {
        LOG_IGNORED,
        LOG_STATUS,
        LOG_UPDATE,
        LOG_MESSAGE,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_FATAL,
    };

   class Logger {
        static constexpr const char* LOG_TYPE_STRINGS[] = {
            "IGNORED",
            "STATUS",
            "UPDATE",
            "MESSAGE",
            "INFO",
            "WARNING",
            "ERROR",
            "FATAL",
        };
        
        public:
            Logger(const std::string& filePath) : _logPath(filePath) {}
            Logger(const std::string& filePath, const bool clearFile) : _logPath(filePath) { if (clearFile) clear(); }
            ~Logger() = default;
           
            inline void log(const std::string& message) const noexcept { log(Log_Status::LOG_INFO, message); }
            virtual void log(const Log_Status status, const std::string& message) const noexcept;

            void set_Log_Path(const std::string& filePath) noexcept { _logPath.assign(filePath); }
            void clear() { FileIO::clear_File(_logPath.c_str()); }

        private:
            static std::string _generate_Log_Message(const Log_Status status, const std::string& message);

            std::string _logPath;
   };

}

#endif // LOVE_LOGGER_HPP
