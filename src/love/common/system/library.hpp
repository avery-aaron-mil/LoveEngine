#ifndef LOVE_LIBRARY_HPP
#define LOVE_LIBRARY_HPP

#ifdef _WIN32
  #include <windows.h>
#endif

namespace love_engine {
    class Library {
#ifdef _WIN32
        typedef HMODULE library_t;
#elif defined(__unix__)
        typedef void* library_t;
#endif
        public:
            Library() = default;
            Library(const char*const library) {
                load_Library(library);
            }
            ~Library() {
                unload_Library();
            }

            void load_Library(const char*const library) noexcept;
            void unload_Library() noexcept;

        private:
            library_t _library = nullptr;
    };
}

#endif // LOVE_LIBRARY_HPP
