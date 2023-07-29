#ifndef LOVE_LOVE_ENGINE_INSTANCE_HPP
#define LOVE_LOVE_ENGINE_INSTANCE_HPP
#include <love/common/love_common.hpp>

#include <love/common/data/files/file_io.hpp>
#include <love/common/system/library.hpp>

#include <functional>

namespace love_engine {
    class LV_ENG_COM LoveEngineInstance {
        public:
            LoveEngineInstance();
            LoveEngineInstance(const std::string& crashDirectory);
            ~LoveEngineInstance();

            static void cleanup() noexcept;
            static void addExitCallback(const std::function<void()>& callback) noexcept;

            static constexpr uint8_t LOVE_ENGINE_VERSION_MAJOR = 1;
            static constexpr uint8_t LOVE_ENGINE_VERSION_MINOR = 0;
            static constexpr uint8_t LOVE_ENGINE_VERSION_PATCH = 0;
        
        private:
            static void _init(const std::string& crashDirectory) noexcept;
    };
}

#endif // LOVE_LOVE_ENGINE_INSTANCE_HPP
