#include "file_io.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#elif defined(__APPLE__)
  #include <cstdlib>
  #include <execinfo.h>
  #include <iterator>
  #include <mach-o/dyld.h>
#elif defined(__unix__)
  #include <cstdlib>
  #include <execinfo.h>
  #include <iterator>
  #include <unistd.h>
#endif

#include "../../error/crash.hpp"

namespace love_engine {
    std::mutex _fileMutex;
    std::string _executable_Directory;

    std::string FileIO::get_Executable_Directory() noexcept {
        if (_executable_Directory.empty()) {
            unsigned int bufferSize = 1024;
            std::vector<char> buffer(bufferSize + 1);
#ifdef _WIN32
            GetModuleFileNameA(NULL, &buffer[0], bufferSize);

#elif defined(__APPLE__)
            if(_NSGetExecutablePath(&buffer[0], &bufferSize)) {
                buffer.resize(bufferSize);
                _NSGetExecutablePath(&buffer[0], &bufferSize);
            }
#else
            // Get the process ID.
            int pid = getpid();

            // Construct a path to the symbolic link pointing to the process executable.
            // This is at /proc/<pid>/exe on Linux systems (we hope).
            UTF8Str link = FormatString::formatString("/proc/%d/exe", pid);

            // Read the contents of the link.
            int count = readlink(link.get(), &buffer[0], bufferSize);
            if(count == -1) {
                Crash::crash("Could not read symbolic link.");
            }
            buffer[count] = '\0';
#endif
            std::string path = &buffer[0];
            std::filesystem::path p = path;
#ifdef __WIN32__
            _executable_Directory.assign(p.parent_path().string() + "\\");
#else
            _executable_Directory.assign(p.parent_path().string() + "/");
#endif
        }
        
        return _executable_Directory;        
    }

    [[nodiscard]] std::string FileIO::remove_Excess_Directory_Slashes(std::string path) noexcept {
        std::string buffer;
        buffer.resize(path.length());
        const char *in = path.data();
        char *out = buffer.data();

        // Copy first character
        *out = *in;

        // Return if empty
        if (*out == '\0') return out;

        for (in++; *in; ++in) {
            if (*out != *in || (*out != '\\' && *out != '/')) {
                // Only copy if not duplicate slash
                *(++out) = *in;
            }
        }

        *(++out) = '\0';
        return buffer;
    }

    void FileIO::ensure_Parent_Directory_Exists(const std::string& path) noexcept {
        if (!std::filesystem::exists(path.c_str())) {
            std::filesystem::path p = path;
            std::filesystem::create_directories(p.parent_path());
        }
    }

    void FileIO::validate_Path(std::string& path) {
        if (path.empty()) throw std::runtime_error("The file path passed is empty.");

        path.assign(remove_Excess_Directory_Slashes(path));
        ensure_Parent_Directory_Exists(path);
    }

    void FileIO::clear_File(std::string filePath) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "wb");
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
        _fileMutex.unlock();
    }

    void FileIO::delete_File(std::string filePath) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        if (std::remove(filePath)) {
            std::stringstream error;
            error << "Could not delete file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
    }

    std::string FileIO::read_File(std::string filePath) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "rb");
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
        if (std::fread(data.data(), 1, size, file) != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not save file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
        return data;
    }
    
    FileIO::FileContent FileIO::read_File_Content(std::string filePath) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "rb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        std::fseek(file , 0 , SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);

        void* data = std::malloc(size);
        if (std::fread(data, 1, size, file) != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
        return FileContent(data, size);
    }
    
    void FileIO::write_File(std::string filePath, const std::string& data) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fwrite(data.data(), 1, data.length(), file) != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
    }

    void FileIO::write_File(std::string filePath, FileIO::FileContent& content) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fwrite(content.data(), 1, content.size(), file) != content.size()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
    }
    
    void FileIO::append_File(std::string filePath, const std::string& data) {
        try {
            validate_Path(filePath);
        } catch (std::runtime_error& e) { throw e; }

        _fileMutex.lock();
        FILE* file = std::fopen(filePath.c_str(), "ab");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }

        if (std::fwrite(data.data(), 1, data.length(), file) != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        _fileMutex.unlock();
    }

    void FileIO::lock() noexcept {
        _fileMutex.lock();
    }
    
    void FileIO::unlock() noexcept {
        _fileMutex.unlock();
    }
}
