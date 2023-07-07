#ifndef LOVE_FILE_COMPRESSION_HPP
#define LOVE_FILE_COMPRESSION_HPP

#include "file_io.hpp"

#include <cstdint>

namespace love_engine {
    class FileCompression {
        public:
            // @throw std::runtime_error If a file error occurs.
            static void compressFile(const char*const filePath, const std::string& content) {
                compressFile(filePath, reinterpret_cast<const uint8_t*const>(content.data()), content.length());
            }
            // @throw std::runtime_error If a file error occurs.
            static void compressFile(const char*const filePath, const FileIO::FileContent& content) {
                compressFile(filePath, reinterpret_cast<const uint8_t*const>(content.data()), content.size());
            }
            // @throw std::runtime_error If a file error occurs.
            static void compressFile(const char*const filePath, const uint8_t*const data, const size_t size);
            // @throw std::runtime_error If a file error occurs.
            static std::string decompressFileString(const char*const filePath);
            // @throw std::runtime_error If a file error occurs.
            static FileIO::FileContent decompressFileRaw(const char*const filePath);
            
            // Level 3 seems to be a good compromise between compression size and speed.
            // Level 2-3 has a noticeable difference in size and moderate increase in compression time.
            // Level 4 has very little increase but double the time of 3.
            static constexpr uint32_t COMPRESSION_PRESET = 3;
    };
}

#endif // LOVE_FILE_COMPRESSION_HPP