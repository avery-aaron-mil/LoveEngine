#include <love/common/data/files/file_io.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
  #include <execinfo.h>
  #include <iterator>
  #include <mach-o/dyld.h>
#elif defined(__unix__)
  #include <execinfo.h>
  #include <iterator>
  #include <unistd.h>
#endif

#include <love/common/error/crash.hpp>
#include <love/common/error/stack_trace.hpp>

namespace love_engine {
    static std::mutex _fileMutex;
    static std::string _executableDirectory;

    std::mutex& FileIO::getMutex() noexcept {
        return _fileMutex;
    }

    std::string FileIO::getExecutableDirectory() noexcept {
        if (_executableDirectory.empty()) {
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
#ifdef __WIN32
            _executableDirectory.assign(p.parent_path().string() + "\\");
#else
            _executableDirectory.assign(p.parent_path().string() + "/");
#endif
        }

        try {
            validatePath(_executableDirectory);
        } catch (std::invalid_argument& e) {
            Crash::crash(std::string("Failed to validate executable path: ") + e.what());
        }
        
        return _executableDirectory;        
    }

    [[nodiscard]] std::string FileIO::removeExcessDirectorySlashes(std::string path) noexcept {
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

    void FileIO::ensureParentDirectoryExists(const std::string& path) noexcept {
        if (!std::filesystem::exists(path.c_str())) {
            std::filesystem::path p = path;
            std::filesystem::create_directories(p.parent_path());
        }
    }

    std::string FileIO::validatePath(std::string& path) {
        if (path.empty()) throw std::invalid_argument(StackTrace::appendStacktrace("The file path passed is empty."));

        path.assign(removeExcessDirectorySlashes(path));
        ensureParentDirectoryExists(path);

        return path;
    }

    void FileIO::clearFile(std::string filePath) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
    }

    void FileIO::deleteFile(std::string filePath) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        if (std::remove(filePath.c_str())) {
            std::stringstream error;
            error << "Could not delete file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
    }

    std::string FileIO::readFile(std::string filePath) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "rb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        std::fseek(file , 0 , SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);
        
        std::string data;
        data.resize(size);
        if (std::fread(data.data(), 1, size, file) != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not save file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
        return data;
    }
    
    FileIO::FileContent FileIO::readFileContent(std::string filePath) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "rb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        std::fseek(file , 0 , SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);

        std::vector<uint8_t> data(size);
        if (std::fread(data.data(), 1, size, file) != size) {
            std::stringstream error;
            error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
        return FileContent(data, size);
    }
    
    void FileIO::writeFile(std::string filePath, const std::string& data) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fwrite(data.data(), 1, data.length(), file) != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
    }

    void FileIO::writeFile(std::string filePath, FileIO::FileContent& content) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "wb");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fwrite(content.data(), 1, content.size(), file) != content.size()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
    }
    
    void FileIO::appendFile(std::string filePath, const std::string& data) {
        try {
            validatePath(filePath);
        } catch (std::invalid_argument& e) { throw e; }

        std::lock_guard<std::mutex> lock(_fileMutex);
        FILE* file = std::fopen(filePath.c_str(), "ab");
        if (!file) {
            std::stringstream error;
            error << "Could not open file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }

        if (std::fwrite(data.data(), 1, data.length(), file) != data.length()) {
            std::stringstream error;
            error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
        
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(StackTrace::appendStacktrace(error));
        }
    }
}
