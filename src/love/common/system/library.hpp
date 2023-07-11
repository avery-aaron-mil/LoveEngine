#ifndef LOVE_LIBRARY_HPP
#define LOVE_LIBRARY_HPP

#ifdef _WIN32
  #include <windows.h>
#endif

namespace love_engine {
    class Library {
#ifdef _WIN32
        typedef HMODULE library_t;
        typedef FARPROC function_t;
#elif defined(__unix__)
        typedef void* library_t;
        typedef void* function_t;
#endif
        public:
            Library() = default;
            Library(const char*const library) {
                loadLibrary(library);
            }
            ~Library() {
                unloadLibrary();
            }

            void loadLibrary(const char*const library) noexcept;
            void unloadLibrary() noexcept;
            function_t loadLibraryFunction(const char*const function) const noexcept;

        private:
            library_t _library = nullptr;
    };
}

#endif // LOVE_LIBRARY_HPP
