#ifndef LOVE_GRAPHICS_DEVICE_HPP
#define LOVE_GRAPHICS_DEVICE_HPP

#include <memory>
#include <string>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW

namespace love_engine {
    class GraphicsDevice {
        public:
            typedef struct Settings_ {
                std::shared_ptr<Logger> logger;
            } Settings;
            
            GraphicsDevice();
            ~GraphicsDevice();

        private:
            Settings _settings;
            VkDevice _device;

            void _log(const std::string& message) const noexcept;
            void _load_Device_Vulkan_Functions() const noexcept;
    };
}

#endif // LOVE_GRAPHICS_DEVICE_HPP
