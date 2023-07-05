#include "library.hpp"

#include "../error/crash.hpp"

#include <string>

#ifdef __unix__
  #include <dlfcn.h>
#endif

namespace love_engine {
    void Library::load_Library(const char*const library) noexcept {
#ifdef _WIN32
            _library = LoadLibraryA(library);
#elif defined(__unix__)
            _library = dlopen(library);
#endif
        if (_library == nullptr) {
            Crash::crash(std::string("Could not load library: ") + library);
        }
    }

    void Library::unload_Library() noexcept {
        if (_library) {
#ifdef _WIN32
            FreeLibrary(_library);
#elif defined(__unix__)
            dlclose(_library);
#endif
            _library = nullptr;
        }
    }
}
