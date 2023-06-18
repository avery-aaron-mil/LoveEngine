#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

#include "file_io.hpp"

#include <future>
#include <string>

namespace love_engine {

    enum class Log_Status {
        LOG_IGNORE,
        LOG_STATUS,
        LOG_UPDATE,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_FATAL,
    };

   class Logger {
        public:
            Logger(const std::string& filePath) : _filePath(filePath) { FileIO::clear_File(filePath.c_str()); }
            ~Logger() {}
           
            void log(const std::string& message) const noexcept { log(Log_Status::LOG_INFO, message); }
            virtual void log(const Log_Status status, const std::string& message) const noexcept;

            void set_File_Path(const std::string& filePath) noexcept { _filePath.assign(filePath); }

        protected:
            // NOTE: Recommended to run this function asynchronously
            static void _logAsync(const std::string&, const std::string& message);

        private:
            std::string _filePath;
   };

}

#endif // LOVE_LOGGER_HPP
