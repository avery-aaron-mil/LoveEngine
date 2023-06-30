#ifndef LOVE_FILE_IO_HPP
#define LOVE_FILE_IO_HPP

#include <cstdlib>
#include <string>

namespace love_engine {
    class FileIO {
        public:
            class FileContent {
                public:
                    FileContent(void*const data, const size_t size) : _data(data), _size(size) {}
                    ~FileContent() { if (_hasAlloc) std::free(_data); }

                    const void*const data() const noexcept  { return _data; }
                    size_t size() const noexcept { return _size; }
                    void set_Alloc(const bool alloc) noexcept { _hasAlloc = alloc; }
 
                private:
                    void* _data;
                    size_t _size;
                    bool _hasAlloc;
            };

            static std::string get_Executable_Directory() noexcept;
            [[nodiscard]] static std::string remove_Excess_Directory_Slashes(std::string path) noexcept;
            static void ensure_Parent_Directory_Exists(const std::string& path) noexcept;
            static void validate_Path(std::string& path);
            static void clear_File(std::string filePath);
            static void delete_File(std::string filePath);
            static std::string read_File(std::string filePath);
            static FileContent read_File_Content(std::string filePath);
            static void write_File(std::string filePath, const std::string& data);
            static void write_File(std::string filePath, FileContent& content);
            static void append_File(std::string filePath, const std::string& data);

            static void lock() noexcept;
            static void unlock() noexcept;
    };
}

#endif // LOVE_FILE_IO_HPP
