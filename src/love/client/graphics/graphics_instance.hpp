#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP

#include "vulkan_instance.hpp"
#include "command_pool.hpp"
#include "frame_buffers.hpp"
#include "graphics_device.hpp"
#include "graphics_pipeline.hpp"
#include "image_views.hpp"
#include "render_pass.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

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
                Window::Properties windowProperties{};
                GraphicsDevice::Properties graphicsDeviceProperties{};
                SwapChain::Properties swapChainProperties{};
                ImageViews::Properties imageViewProperties{};
                RenderPass::Properties renderPassProperties{};
                GraphicsPipeline::Properties graphicsPipelineProperties{};
                FrameBuffers::Properties frameBufferProperties{};
                CommandPool::Properties commandPoolProperties{};

                std::function<void(int, const char*)> glfwErrorCallback = _defaultGLFWErrorCallback;

                std::vector<VkCommandBufferBeginInfo> commandBufferBeginInfo;
                std::vector<VkRenderPassBeginInfo> renderPassBeginInfo;
            };

            GraphicsInstance(const Properties& properties, std::shared_ptr<Logger> logger);
            ~GraphicsInstance() = default;

            inline const Window& window() const noexcept { return _window; }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;

            VulkanInstance _vulkanInstance {
                _properties.applicationInfo,
                _properties.glfwErrorCallback,
                _logger
            };
            Window _window {
                _vulkanInstance.instance(),
                _properties.windowProperties,
                _logger
            };
            GraphicsDevice _graphicsDevice {
                _vulkanInstance.instance(),
                _window.surface(),
                _properties.graphicsDeviceProperties,
                _logger
            };
            SwapChain _swapChain {
                _graphicsDevice,
                _window,
                _properties.swapChainProperties,
                _logger
            };
            ImageViews _imageViews {
                _graphicsDevice.device(),
                _swapChain.swapChainImages(),
                _swapChain.imageFormat(),
                _properties.imageViewProperties,
                _logger
            };
            RenderPass _renderPass {
                _graphicsDevice.device(),
                _swapChain.imageFormat(),
                _properties.renderPassProperties,
                _logger
            };
            GraphicsPipeline _graphicsPipeline {
                _graphicsDevice.device(),
                _renderPass.renderPass(),
                _window.extent(),
                _properties.graphicsPipelineProperties,
                _logger
            };
            FrameBuffers _frameBuffers {
                _graphicsDevice.device(),
                _renderPass.renderPass(),
                _window.extent(),
                _imageViews.imageViews(),
                _properties.frameBufferProperties,
                _logger
            };
            CommandPool _commandPool {
                _graphicsDevice.device(),
                _renderPass.renderPass(),
                _graphicsDevice.queueFamilyIndices(),
                _properties.commandPoolProperties,
                _logger
            };

            void _log(const std::string& message) const noexcept;
            static void _defaultGLFWErrorCallback(int error, const char* description);
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
