#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

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
            Logger(const std::string& filePath) : _filePath(filePath) {
#ifdef _WIN32
                setConsole();
#endif
            }
            ~Logger() {}
           
            virtual void log(const std::string& message) const noexcept;
            virtual void log(const Log_Status status, const std::string& message) const noexcept;

        private:
            std::string _filePath;

#ifdef _WIN32
            void setConsole();
#endif
   };

}

#endif // LOVE_LOGGER_HPP
