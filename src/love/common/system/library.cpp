#include <love/common/system/library.hpp>

#include <love/common/error/crash.hpp>

#include <sstream>
#include <string>

#ifdef _WIN32
#include <love/common/error/win32_error.hpp>
#elif defined(__unix__)
  #include <dlfcn.h>
#endif

namespace love_engine {
    Library::~Library() {
        unloadLibrary();
    }

    std::string _getError() {
#ifdef _WIN32
        return Win32Error::getLastError();
#elif defined(__unix__)
        const char* errorMsg = std::dlerror();
        return std::string(errorMsg);
#endif
    }

    void Library::loadLibrary(const char*const library) noexcept {
        if (_library) unloadLibrary();
#ifdef _WIN32
            _library = LoadLibraryA(library);
#elif defined(__unix__)
            _library = std::dlopen(library);
#endif
        if (_library == nullptr) {
            std::stringstream buffer;
            buffer << "Could not load library \"" << library << "\": " << _getError();
            Crash::crash(buffer.str());
        }
    }

    void Library::unloadLibrary() noexcept {
        if (_library) {
#ifdef _WIN32
            FreeLibrary(_library);
#elif defined(__unix__)
            std::dlclose(_library);
#endif
            _library = nullptr;
        }
    }

    Library::function_t Library::loadLibraryFunction(const char*const function) const noexcept {
        if (_library == nullptr) {
            std::stringstream buffer;
            buffer << "Cannot load library functions before loading library: " << function;
            Crash::crash(buffer.str());
        }
#ifdef _WIN32
        function_t address = GetProcAddress(_library, function);
#elif defined(__unix__)
        function_t address = std::dlsym(_library, function);
#endif
        if (address == nullptr) {
            std::stringstream buffer;
            buffer << "Could not load library function \"" << function << "\": " << _getError();
            Crash::crash(buffer.str());
        }

        return address;
    }
}
