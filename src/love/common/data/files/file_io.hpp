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
                    ~FileContent() { std::free(_data); }

                    const void*const data() const noexcept  { return _data; }
                    size_t size() const noexcept { return _size; }

                private:
                    void* _data;
                    size_t _size;
            };

            static void clear_File(const char*const filePath);
            static std::string read_File(const char*const filePath);
            static FileContent read_File_Content(const char*const filePath);
            static void write_File(const char*const filePath, const std::string& data);
            static void write_File(const char*const filePath, FileContent& content);
            static void append_File(const char*const filePath, const std::string& data);

            static void lock();
            static void unlock();
    };
}

#endif // LOVE_FILE_IO_HPP
