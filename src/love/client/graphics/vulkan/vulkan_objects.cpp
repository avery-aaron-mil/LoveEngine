#include <love/client/graphics/vulkan/vulkan_objects.hpp>

#include <love/common/error/crash.hpp>

namespace love_engine {
    VulkanObjects::VulkanObjects(VkInstance instance, const Properties& properties, std::shared_ptr<Logger> logger)
    : _logger(logger), _properties(properties), _vulkanInstance(instance) {
        if (_vulkanInstance == nullptr) Crash::crash("Vulkan instance passed to VulkanObjects was null.");
    }
    VulkanObjects::~VulkanObjects() {
        _log("Destroying Vulkan objects...");
        if (_commandPool.get()) _commandPool.reset();
        if (_frameBuffers.get()) _frameBuffers.reset();
        if (_graphicsPipeline.get()) _graphicsPipeline.reset();
        if (_renderPass.get()) _renderPass.reset();
        if (_imageViews.get()) _imageViews.reset();
        if (_swapChain.get()) _swapChain.reset();
        if (_window.get()) _window.reset();
        if (_graphicsDevice.get()) _graphicsDevice.reset();
        _log("Destroyed Vulkan objects.");
    }

    void VulkanObjects::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/VulkanObjects): " + message);
        }
    }

    void VulkanObjects::createVulkanObjects() noexcept {
        _log("Creating Vulkan objects...");

        _window = std::make_unique<Window>(
            _vulkanInstance,
            _properties.windowProperties,
            _logger
        );
        if (_window.get() == nullptr) Crash::crash("Failed to create window object.");

        _graphicsDevice = std::make_unique<GraphicsDevice>(
            _vulkanInstance,
            _window.get()->surface(),
            _properties.graphicsDeviceProperties,
            _logger
        );
        if (_graphicsDevice.get() == nullptr) Crash::crash("Failed to create graphics device object.");

        _swapChain = std::make_unique<SwapChain>(
            _graphicsDevice.get(),
            _window.get(),
            _properties.swapChainProperties,
            _logger
        );
        if (_swapChain.get() == nullptr) Crash::crash("Failed to create swap chain object.");

        _imageViews = std::make_unique<ImageViews>(
            _graphicsDevice.get()->device(),
            _swapChain.get()->swapChainImages(),
            _swapChain.get()->imageFormat(),
            _properties.imageViewProperties,
            _logger
        );
        if (_imageViews.get() == nullptr) Crash::crash("Failed to create image views object.");

        _renderPass = std::make_unique<RenderPass>(
            _graphicsDevice.get()->device(),
            _swapChain.get()->imageFormat(),
            _properties.renderPassProperties,
            _logger
        );
        if (_renderPass.get() == nullptr) Crash::crash("Failed to create render pass object.");

        _graphicsPipeline = std::make_unique<GraphicsPipeline>(
            _graphicsDevice.get()->device(),
            _renderPass.get()->renderPass(),
            _window.get()->extent(),
            _properties.graphicsPipelineProperties,
            _logger
        );
        if (_graphicsPipeline.get() == nullptr) Crash::crash("Failed to create pipeline object.");

        _frameBuffers = std::make_unique<FrameBuffers>(
            _graphicsDevice.get()->device(),
            _renderPass.get()->renderPass(),
            _window.get()->extent(),
            _imageViews.get()->imageViews(),
            _properties.frameBufferProperties,
            _logger
        );
        if (_frameBuffers.get() == nullptr) Crash::crash("Failed to create frame buffers object.");

        _commandPool = std::make_unique<CommandPool>(
            _graphicsDevice.get()->device(),
            _renderPass.get()->renderPass(),
            _graphicsDevice.get()->queueFamilyIndices(),
            _properties.commandPoolProperties,
            _logger
        );
        if (_commandPool.get() == nullptr) Crash::crash("Failed to create command pool object.");

        _log("Created Vulkan objects.");
    }
}
