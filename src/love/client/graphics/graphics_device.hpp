#ifndef LOVE_GRAPHICS_DEVICE_HPP
#define LOVE_GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW

#include "graphics_instance.hpp"

namespace love_engine {
    class GraphicsDevice {
        public:
            GraphicsDevice(std::shared_ptr<Logger> logger, VkInstance vulkanInstance);
            ~GraphicsDevice();

        private:
            std::shared_ptr<Logger> logger;
            VkInstance _vulkanInstance;
            VkDevice _device;
            std::vector<VkPhysicalDevice> _physicalDevices;

            void _log(const std::string& message) const noexcept;
            void _get_Physical_Devices() noexcept;
            int64_t _rate_Device(const VkPhysicalDevice& device) noexcept();
            void _create_Device() noexcept;
            void _load_Device_Vulkan_Functions() const noexcept;
    };
}

#endif // LOVE_GRAPHICS_DEVICE_HPP
