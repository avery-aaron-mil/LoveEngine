#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP
#include <love/client/love_client.hpp>

#include <love/client/graphics/vulkan/vulkan_instance.hpp>
#include <love/common/data/files/logger.hpp>

#include <memory>
#include <string>

namespace love_engine {
    class LV_ENG_CLI GraphicsInstance {
        public:
            struct Properties {
                VulkanInstance::ApplicationInfo applicationInfo{};
                VulkanInstance::Properties instanceProperties{};
            };

            GraphicsInstance(const Properties& properties, std::shared_ptr<Logger> logger);
            ~GraphicsInstance() = default;

            inline Window* window() const noexcept { return _vulkanInstance.get()->window(); }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;

            std::unique_ptr<VulkanInstance> _vulkanInstance = nullptr;

            void _log(const std::string& message) const noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
