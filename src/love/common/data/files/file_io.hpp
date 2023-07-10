#ifndef LOVE_FILE_IO_HPP
#define LOVE_FILE_IO_HPP

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace love_engine {
    class FileIO {
        public:
            class FileContent {
                public:
                    FileContent(std::vector<uint8_t> data, const size_t size) : _data(data), _size(size) {}
                    ~FileContent() = default;

                    const uint8_t*const data() const noexcept  { return _data.data(); }
                    size_t size() const noexcept { return _size; }
 
                private:
                    std::vector<uint8_t> _data;
                    size_t _size;
            };

            static std::string getExecutableDirectory() noexcept;
            [[nodiscard]] static std::string removeExcessDirectorySlashes(std::string path) noexcept;
            static void ensureParentDirectoryExists(const std::string& path) noexcept;
            // Makes @p path valid if it is not already.
            // @throw std::invalid_argument If @p path is empty.
            static void validatePath(std::string& path);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void clearFile(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void deleteFile(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static std::string readFile(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static FileContent readFileContent(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void writeFile(std::string filePath, const std::string& data);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void writeFile(std::string filePath, FileContent& content);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void appendFile(std::string filePath, const std::string& data);

            static std::mutex& getMutex() noexcept;
    };
}

#endif // LOVE_FILE_IO_HPP
