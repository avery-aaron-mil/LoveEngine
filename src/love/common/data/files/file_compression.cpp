#include "file_compression.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <lzma.h>

namespace love_engine {
    void _initEncoder(lzma_stream* stream, const char*__restrict__ filePath) {
        lzma_mt mt = {
            .flags = 0,
            .threads = std::min(std::thread::hardware_concurrency(), 8u),
            .block_size = 0,
            .timeout = 0,
            .preset = FileCompression::COMPRESSION_PRESET,
            .filters = nullptr, // Filters must be null with a preset
            .check = LZMA_CHECK_CRC64,
        };

        lzma_ret ret = lzma_stream_encoder_mt(stream, &mt);

        if (ret != LZMA_OK) {
            switch (ret) {
                case LZMA_MEM_ERROR: {
                    throw std::runtime_error(std::string("Ran out of memory while decompressing file: ") + filePath);
                }

                case LZMA_OPTIONS_ERROR: {
                    throw std::runtime_error(std::string("Unsupported decompressor flags for file: ") + filePath);
                }
                
                case LZMA_UNSUPPORTED_CHECK: {
                    throw std::runtime_error(std::string("Unsupported integrity check for file: ") + filePath);
                }

                default: {
                    throw std::runtime_error(std::string("Unknown error occurred while decompressing file: ") + filePath);
                }
            }
        }
    }
    
    void _initDecoder(lzma_stream *stream, const char*__restrict__ filePath) {
        // The .xz format allows concatenating compressed files as is:
        //
        //     echo foo | xz > foobar.xz
        //     echo bar | xz >> foobar.xz
        //
        // When decompressing normal standalone .xz files, LZMA_CONCATENATED
        // should always be used to support decompression of concatenated
        // .xz files. If LZMA_CONCATENATED isn't used, the decoder will stop
        // after the first .xz stream. This can be useful when .xz data has
        // been embedded inside another file format.
        //
        // Flags other than LZMA_CONCATENATED are supported too, and can
        // be combined with bitwise-or. See lzma/container.h
        // (src/liblzma/api/lzma/container.h in the source package or e.g.
        // /usr/include/lzma/container.h depending on the install prefix)
        // for details.
        lzma_ret ret = lzma_stream_decoder(stream, UINT64_MAX, LZMA_CONCATENATED);

        if (ret != LZMA_OK) {
            switch (ret) {
                case LZMA_MEM_ERROR: {
                    throw std::runtime_error(std::string("Ran out of memory while decompressing file: ") + filePath);
                }

                case LZMA_OPTIONS_ERROR: {
                    throw std::runtime_error(std::string("Unsupported decompressor flags for file: ") + filePath);
                }

                default: {
                    throw std::runtime_error(std::string("Unknown error occurred while decompressing file: ") + filePath);
                }
            }
        }
    }
    
    void FileCompression::compress_File(const char*const filePath, const uint8_t*const data, const size_t size) {
        lzma_stream stream = LZMA_STREAM_INIT;
        _initEncoder(&stream, filePath);
	    lzma_action action = LZMA_RUN;

        // Open file
        FileIO::lock();
        FILE* file = fopen(filePath, "wb");
        if (!file) throw std::runtime_error(std::string("Error opening file: ") + filePath);

        size_t head = 0, charsWritten = 0;
        uint8_t buf[BUFSIZ];
        
        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = buf;
        stream.avail_out = sizeof(buf);

        // Compress and write
        while (true) {
            // Fill the input buffer if it is empty.
            if (stream.avail_in == 0 && head < size) {
                charsWritten = std::min(sizeof(buf), size - head);
                stream.next_in = data + head;
                stream.avail_in = charsWritten;
                head += charsWritten;

                if (head >= size) action = LZMA_FINISH;
            }

            // Compress
		    lzma_ret ret = lzma_code(&stream, action);
            
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                // When lzma_code() has returned LZMA_STREAM_END, the output buffer is likely to be only
                // partially full. Calculate how much new data there is to be written to the output file.
                charsWritten = sizeof(buf) - stream.avail_out;

                if (std::fwrite(buf, 1, charsWritten, file) != charsWritten) {
                    std::stringstream error;
                    error << "Could not write to file \"" << filePath << "\": " << std::strerror(errno);
                    throw std::runtime_error(error.str());
                }

                // Reset next_out and avail_out.
                stream.next_out = buf;
                stream.avail_out = sizeof(buf);
            }

            if (ret != LZMA_OK) {
                // Once everything has been encoded successfully, the return value of
                // lzma_code() will be LZMA_STREAM_END.
                if (ret == LZMA_STREAM_END) break;

                switch (ret) {
                    case LZMA_MEM_ERROR: {
                        throw std::runtime_error(std::string("Ran out of memory while compressing file: ") + filePath);
                    }

                    case LZMA_DATA_ERROR: {
                        throw std::runtime_error(std::string("File size is greater than maximum (2^63 bytes): ") + filePath);
                    }

                    default: {
                        throw std::runtime_error(std::string("Unknown error occurred while compressing file: ") + filePath);
                    }
                }
            }
        }

        // Close file
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        FileIO::unlock();
	    lzma_end(&stream);
    }

    std::string FileCompression::decompress_File_String(const char*const filePath) {
        FileIO::FileContent content = decompress_File_Raw(filePath);
        std::string strContent;
        strContent.resize(content.size());
        std::memmove(strContent.data(), content.data(), content.size());
        content.set_Alloc(false);
        return strContent;
    }
    
    FileIO::FileContent FileCompression::decompress_File_Raw(const char*const filePath) {
        lzma_stream stream = LZMA_STREAM_INIT;
        _initDecoder(&stream, filePath);
	    lzma_action action = LZMA_RUN;

        // Open file
        FileIO::lock();
        FILE* file = std::fopen(filePath, "rb");
        if (!file) throw std::runtime_error(std::string("Error opening file: ") + filePath);
        
        size_t head = 0, charsRead = 0;
        uint8_t inbuf[BUFSIZ], outbuf[BUFSIZ];
        size_t capacity = sizeof(inbuf);
        void* data = std::malloc(capacity);

        stream.next_in = nullptr;
        stream.avail_in = 0;
        stream.next_out = outbuf;
        stream.avail_out = sizeof(outbuf);

        while (true) {
            // Fill input buffer
            if (stream.avail_in == 0 && !std::feof(file)) {
                stream.next_in = inbuf;
                stream.avail_in = std::fread(inbuf, 1, sizeof(inbuf), file);
                if (stream.avail_in != sizeof(inbuf)) {
                    if (std::feof(file)) action = LZMA_FINISH;
                    else {
                        std::stringstream error;
                        error << "Could not read from file \"" << filePath << "\": " << std::strerror(errno);
                        throw std::runtime_error(error.str());
                    }
                }

                if (std::feof(file)) action = LZMA_FINISH;
            }

            // Decompress
            lzma_ret ret = lzma_code(&stream, action);

            // Fill output data
            if (stream.avail_out == 0 || ret == LZMA_STREAM_END) {
                charsRead = sizeof(outbuf) - stream.avail_out;
                head += charsRead;
                
                // Check if data can fit buffer
                if (capacity < head) {
                    capacity = head * 2;
                    data = std::realloc(data, capacity);
                }
                std::memcpy(reinterpret_cast<uint8_t*>(data) + (head - charsRead), outbuf, charsRead);
                
                stream.next_out = outbuf;
                stream.avail_out = sizeof(outbuf);
            }

            if (ret != LZMA_OK) {
                if (ret == LZMA_STREAM_END) break;

                switch (ret) {
                    case LZMA_MEM_ERROR: {
                        throw std::runtime_error(std::string("Ran out of memory while decompressing file: ") + filePath);
                    }

                    case LZMA_DATA_ERROR: {
                        throw std::runtime_error(std::string("Corrupted data encountered while decompressing file: ") + filePath);
                    }

                    case LZMA_FORMAT_ERROR: {
                        throw std::runtime_error(std::string("Input file for decompression is not in xz format: ") + filePath);
                    }

                    case LZMA_OPTIONS_ERROR: {
                        throw std::runtime_error(std::string("Unsupported compression options for file: ") + filePath);
                    }

                    case LZMA_BUF_ERROR: {
                        throw std::runtime_error(std::string("Compressed file is truncated or otherwise corrupt: ") + filePath);
                    }

                    default: {
                        throw std::runtime_error(std::string("Unknown error occurred while decompressing file: ") + filePath);
                    }
                }
            }
	    }

        // Close file
        if (std::fclose(file)) {
            std::stringstream error;
            error << "Could not close file \"" << filePath << "\": " << std::strerror(errno);
            throw std::runtime_error(error.str());
        }
        FileIO::unlock();
	    lzma_end(&stream);
	    data = std::realloc(data, head);
        return FileIO::FileContent(data, head);
    }
}
