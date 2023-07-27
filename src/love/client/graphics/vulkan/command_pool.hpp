#ifndef LOVE_COMMAND_POOL_HPP
#define LOVE_COMMAND_POOL_HPP

#include <memory>
#include <string>
#include <vector>

#include <love/client/graphics/vulkan/graphics_device.hpp>
#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class CommandPool {
        public:
            struct Properties {
                std::shared_ptr<VkCommandPoolCreateInfo> commandPoolInfo = nullptr;
                std::shared_ptr<VkCommandBufferAllocateInfo> commandBufferAllocateInfo = nullptr;
                uint32_t commandBufferCount = 1;
            };

            CommandPool(
                VkDevice device,
                VkRenderPass renderPass,
                const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~CommandPool();

            inline VkCommandPool commandPool() const noexcept { return _commandPool; }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkDevice _device = nullptr;
            VkRenderPass _renderPass = nullptr;
            VkCommandPool _commandPool = nullptr;
            std::vector<VkCommandBuffer> _commandBuffers;

            void _log(const std::string& message) const noexcept;
            void _createCommandPool(const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices) noexcept;
            void _allocateCommandBuffers() noexcept;
    };
}

#endif // LOVE_COMMAND_POOL_HPP
