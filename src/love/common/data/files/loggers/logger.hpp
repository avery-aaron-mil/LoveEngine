#ifndef LOVE_LOGGER_HPP
#define LOVE_LOGGER_HPP

#include "../../string/utf8_str.hpp"

namespace love_engine {

   class Logger {
       public:
           Logger() {}
           ~Logger() {}
           
           virtual void log(const UTF8_Str& message) = 0; 
   };

}

#endif // LOVE_LOGGER_HPP
