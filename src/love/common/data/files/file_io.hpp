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

            static std::string get_Executable_Directory() noexcept;
            [[nodiscard]] static std::string remove_Excess_Directory_Slashes(std::string path) noexcept;
            static void ensure_Parent_Directory_Exists(const std::string& path) noexcept;
            // @throw std::invalid_argument If @p path is empty.
            static void validate_Path(std::string& path);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void clear_File(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void delete_File(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static std::string read_File(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static FileContent read_File_Content(std::string filePath);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void write_File(std::string filePath, const std::string& data);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void write_File(std::string filePath, FileContent& content);
            // @throw std::invalid_argument If @p filePath is empty.
            // @throw std::runtime_error If a file error occurs.
            static void append_File(std::string filePath, const std::string& data);

            static std::mutex& get_Mutex() noexcept;
    };
}

#endif // LOVE_FILE_IO_HPP
