#ifndef LOVE_LOVE_ENGINE_INSTANCE_HPP
#define LOVE_LOVE_ENGINE_INSTANCE_HPP

#include "data/files/file_io.hpp"

#include <functional>

namespace love_engine {
    class LoveEngineInstance {
        public:
            static inline void init() noexcept { init(FileIO::get_Executable_Directory() + "crash-reports"); }
            static void init(const std::string& crashDirectory) noexcept;
            static void cleanup() noexcept;
            static void add_Exit_Callback(std::function<void()> callback);
    };
}

#endif // LOVE_LOVE_ENGINE_INSTANCE_HPP
