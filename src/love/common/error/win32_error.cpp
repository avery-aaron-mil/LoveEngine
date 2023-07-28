#include <love/common/error/win32_error.hpp>

namespace love_engine {
    std::string Win32Error::getSystemMessage(LSTATUS resultCode) noexcept {
        LPSTR systemMessageBuffer = nullptr;
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
            0,
            resultCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            systemMessageBuffer,
            0,
            nullptr
        );
        std::string systemMessage(systemMessageBuffer);
        LocalFree(systemMessageBuffer);
        return systemMessage;
    }
}
