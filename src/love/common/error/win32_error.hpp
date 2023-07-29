#ifdef _WIN32
#ifndef LOVE_WIN32_ERROR_HPP
#define LOVE_WIN32_ERROR_HPP
#include <love/common/love_common.hpp>

#include <string>

#include <windows.h>

namespace love_engine {
    class LV_ENG_COM Win32Error {
        public:
            static std::string getLastError() noexcept;
            static std::string getSystemMessage(LSTATUS resultCode) noexcept;
    };
}

#endif // LOVE_WIN32_ERROR_HPP
#endif // _WIN32
