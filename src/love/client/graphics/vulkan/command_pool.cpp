#include <love/client/graphics/vulkan/command_pool.hpp>

#include <sstream>

#include <love/client/graphics/vulkan/vulkan_functions.hpp>
#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

namespace love_engine {
    CommandPool::CommandPool(
        VkDevice device,
        VkRenderPass renderPass,
        const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _properties(properties), _device(device), _renderPass(renderPass) {
        if (_device == nullptr) Crash::crash("Vulkan device to command pool was null.");
        if (_renderPass == nullptr) Crash::crash("Render pass passed to command pool was null.");

        _createCommandPool(queueFamilyIndices);
        _allocateCommandBuffers();
    }
    CommandPool::~CommandPool() {
        if (_commandPool) vkDestroyCommandPool(_device, _commandPool, nullptr);
    }

    void CommandPool::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/CommandPool): " + message);
        }
    }

    void CommandPool::_createCommandPool(const GraphicsDevice::QueueFamilyIndices& queueFamilyIndices) noexcept {
        _log("Creating command pool...");

        if (_properties.commandPoolInfo.get() == nullptr) {
            VkCommandPoolCreateInfo commandPoolInfo {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamilyIndices.graphicsQueue
            };

            _properties.commandPoolInfo = std::make_shared<VkCommandPoolCreateInfo>(commandPoolInfo);
        }

        auto result = vkCreateCommandPool(_device, _properties.commandPoolInfo.get(), nullptr, &_commandPool);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to create command pool: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        _log("Created command pool.");
    }

    void CommandPool::_allocateCommandBuffers() noexcept {
        _log("Allocating command buffers...");

        if (_properties.commandBufferAllocateInfo.get() == nullptr) {
            VkCommandBufferAllocateInfo allocInfo {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = _commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = _properties.commandBufferCount
            };

            _properties.commandBufferAllocateInfo = std::make_shared<VkCommandBufferAllocateInfo>(allocInfo);
        }

        _commandBuffers.resize(_properties.commandBufferAllocateInfo.get()->commandBufferCount);
        auto result = vkAllocateCommandBuffers(_device, _properties.commandBufferAllocateInfo.get(), _commandBuffers.data());
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to allocate command buffers: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        _log("Allocated command buffers.");
    }
}
