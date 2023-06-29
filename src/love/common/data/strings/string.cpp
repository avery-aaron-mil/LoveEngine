#include "string.hpp"

#include <cstdlib>
#include <cstring>

namespace love_engine {
    std::string& String::reverse(std::string& str) {
        const size_t len = str.length();
        char *const dst = str.data();

        char temp;
        // Replace working outside of string to the middle
        for (size_t i = 0; i < len / 2; i++) {
            temp = dst[i];
            dst[i] = dst[len - i - 1];
            dst[len - i - 1] = temp;
        }
        
        return str;
    }
    
    std::string& String::insert(std::string& str, const char c, const size_t pos) noexcept {
        const size_t len = str.length();
        // Input validdation
        if (pos > len) return str;
        
        str.resize(len + 1);
        char *const dst = str.data();
        // Copy character to be replaced
        char temp = dst[pos];

        // Insert at pos
        dst[pos] = c;

        // Copy remaining characters
        char temp2;
        for (size_t i = pos + 1; i < len + 1; i++) {
            temp2 = dst[i];
            dst[i] = temp;
            temp = temp2;
        }
        
        return str;
    }

    std::string& String::insert(std::string& str, const std::string& insertStr, const size_t pos) noexcept {
        const size_t strLen = str.length();
        // Input validation
        if ((pos > strLen) || insertStr.empty()) return str;

        const size_t insertLen = insertStr.length();
        str.resize(strLen + insertLen);
        char *const dst = str.data();

        // Copy str to buffer after pos
        const size_t rem = strLen - pos;
        char* buffer = static_cast<char*>(std::malloc(rem));
        std::memcpy(buffer, dst + pos, rem);

        // Insert at pos
        size_t currentLen = pos;
        std::memcpy(dst + currentLen, insertStr.c_str(), insertLen);
        currentLen += insertLen;

        // Copy remaining characters
        std::memcpy(dst + currentLen, buffer, rem);

        std::free(buffer);
        return str;
    }
    
    bool String::is_ASCII(const std::string& str) noexcept {
        const size_t len = str.length();
        char *const data = str.data();

        char c;
        for (size_t i = 0; i < len; i++) {
            c = data[i];
            if (c < ' ' || c > '~') return false;
        }

        return true;
    }

    char String::translate_Escape_Character(const char c) noexcept {
        switch (c) {
            case '\"': return '\"';
            case '\'': return '\'';
            case '\\': return '\\';
            case '0': return '\0';
            case '1': return '\1';
            case '2': return '\2';
            case '3': return '\3';
            case '4': return '\4';
            case '5': return '\5';
            case '6': return '\6';
            case '7': return '\7';
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            default: return -1;
        }
    }
}
