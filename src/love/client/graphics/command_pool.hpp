#ifndef LOVE_COMMAND_POOL_HPP
#define LOVE_COMMAND_POOL_HPP

#include <memory>
#include <string>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

#include "graphics_device.hpp"

namespace love_engine {
    class CommandPool {
        public:
            struct Properties {
                std::shared_ptr<VkCommandPoolCreateInfo> commandPoolInfo = nullptr;
                std::vector<VkCommandBufferAllocateInfo> commandBufferAllocateInfo;
            };

            CommandPool(
                VkDevice device,
                VkRenderPass renderPass,
                const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~CommandPool();

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkDevice _device = nullptr;
            VkRenderPass _renderPass = nullptr;
            VkCommandPool _commandPool = nullptr;

            void _log(const std::string& message) const noexcept;
            void _createCommandPool(const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices) noexcept;
    };
}

#endif // LOVE_COMMAND_POOL_HPP
