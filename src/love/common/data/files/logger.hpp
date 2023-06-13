#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

#include <string>

namespace love_engine {

    enum Log_Status {
        IGNORE,
        STATUS,
        UPDATE,
        INFO,
        WARNING,
        ERROR,
        FATAL,
    };

   class Logger {
        public:
            Logger(const std::string& filePath) : _filePath(filePath) {}
            ~Logger() {}
           
            virtual void log(const std::string& message) const noexcept;
            virtual void log(const Log_Status status, const std::string& message) const noexcept;

        private:
            std::string _filePath;
   };

}

#endif // LOVE_LOGGER_HPP
