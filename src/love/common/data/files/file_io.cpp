#include "file_io.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace love_engine {
    std::mutex fileMutex;

    void FileIO::clear_File(const char*const filePath) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
    }

    std::string FileIO::read_File(const char*const filePath) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "rb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        std::fseek(file , 0 , SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);
        
        std::string data;
        data.resize(size);
        size_t result = std::fread(data.data(), 1, size, file);
        if (result != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not save file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
        return data;
    }
    
    FileIO::FileContent FileIO::read_File_Content(const char*const filePath) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "rb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        std::fseek(file , 0 , SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);

        void* data = std::malloc(size);
        size_t result = std::fread(data, 1, size, file);
        if (result != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
        return FileContent(data, size);
    }
    
    void FileIO::write_File(const char*const filePath, const std::string& data) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "wb");
        if (!file) { // TODO Make directories if required
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        size_t result = std::fwrite(data.data(), 1, data.length(), file);
        if (result != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
    }

    void FileIO::write_File(const char*const filePath, FileIO::FileContent& content) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        size_t result = std::fwrite(content.data(), 1, content.size(), file);
        if (result != content.size()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
    }
    
    void FileIO::append_File(const char*const filePath, const std::string& data) {
        fileMutex.lock();
        FILE* file = std::fopen(filePath, "ab");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        size_t result = std::fwrite(data.data(), 1, data.length(), file);
        if (result != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        fileMutex.unlock();
    }
}
