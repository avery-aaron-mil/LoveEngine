#ifndef LOVE_STRING_HPP
#define LOVE_STRING_HPP

#include <cstdint>
#include <string>

namespace love_engine {
    class String {
        public:
            static std::string& reverse(std::string& str);

            static std::string& insert(std::string& str, const char c, const size_t pos) noexcept;
            static std::string& insert(std::string& str, const std::string& insertStr, const size_t pos) noexcept;

            static std::string& prepend(std::string& str, const std::string& prependStr) {
                return insert(str, prependStr, 0);
            }

            static bool is_ASCII(const std::string& str) noexcept;

            static char translate_Escape_Character(const char c) noexcept;
    };
}

#endif // LOVE_STRING_HPP
