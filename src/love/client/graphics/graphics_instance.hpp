#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP

#include <love/client/graphics/vulkan/vulkan_instance.hpp>
#include <love/common/data/files/logger.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace love_engine {
    class GraphicsInstance {
        public:
            struct Properties {
                VulkanInstance::ApplicationInfo applicationInfo{};
                VulkanInstance::Properties instanceProperties{};
            };

            GraphicsInstance(const Properties& properties, std::shared_ptr<Logger> logger);
            ~GraphicsInstance() = default;

            inline Window* window() const noexcept { return _vulkanInstance.window(); }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;

            VulkanInstance _vulkanInstance {
                _properties.applicationInfo,
                _properties.instanceProperties,
                _logger
            };

            void _log(const std::string& message) const noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
