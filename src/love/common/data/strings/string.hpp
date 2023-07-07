#ifndef LOVE_STRING_HPP
#define LOVE_STRING_HPP

#include <cstdint>
#include <string>

namespace love_engine {
    class String {
        public:
            static std::string& reverse(std::string& str);

            // @throw std::length_error If @p pos > @p str.length().
            static std::string& insert(std::string& str, const size_t pos, const char c);
            // @throw std::length_error If @p pos > @p str.length().
            static std::string& insert(std::string& str, const size_t pos, size_t count, const char c);
            // @throw std::length_error If @p pos > @p str.length().
            static std::string& insert(std::string& str, const size_t pos, const std::string& insertStr);

            static std::string& prepend(std::string& str, const std::string& prependStr) noexcept {
                return insert(str, 0, prependStr);
            }

            static bool isASCII(const std::string& str) noexcept;

            static char translateEscapeCharacter(const char c) noexcept;
    };
}

#endif // LOVE_STRING_HPP
