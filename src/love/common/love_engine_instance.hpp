#ifndef LOVE_LOVE_ENGINE_INSTANCE_HPP
#define LOVE_LOVE_ENGINE_INSTANCE_HPP

#include "data/files/file_io.hpp"

#include <functional>

namespace love_engine {
    class LoveEngineInstance {
        public:
            static inline void init() noexcept { init(FileIO::getExecutableDirectory() + "crash-reports"); }
            static void init(const std::string& crashDirectory) noexcept;
            static void cleanup() noexcept;
            static void addExitCallback(const std::function<void()>& callback) noexcept;

            static constexpr uint8_t LOVE_ENGINE_VERSION_MAJOR = 1;
            static constexpr uint8_t LOVE_ENGINE_VERSION_MINOR = 0;
            static constexpr uint8_t LOVE_ENGINE_VERSION_PATCH = 0;
    };
}

#endif // LOVE_LOVE_ENGINE_INSTANCE_HPP
