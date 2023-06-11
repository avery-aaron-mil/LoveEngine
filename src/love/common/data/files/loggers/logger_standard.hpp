#ifndef LOVE_STANDARD_LOGGER_HPP
#define LOVE_STANDARD_LOGGER_HPP

#include "logger.hpp"

namespace love_engine {

   class Standard_Logger : public Logger {
       public:
           Standard_Logger() {}
           ~Standard_Logger() {}
           
           virtual void log(const UTF8_Str& message) noexcept;
   };

}

#endif // LOVE_STANDARD_LOGGER_HPP
