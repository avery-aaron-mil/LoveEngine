#ifndef LOVE_LOVE_ENGINE_INSTANCE_HPP
#define LOVE_LOVE_ENGINE_INSTANCE_HPP

#include "data/files/file_io.hpp"

namespace love_engine {
    class LoveEngineInstance {
        public:
            static inline void init() { init(FileIO::get_Executable_Directory() + "crash-reports"); }
            static void init(const std::string& crashDirectory);
            static void cleanup();
    };
}

#endif // LOVE_LOVE_ENGINE_INSTANCE_HPP
