#ifndef LOVE_VULKAN_OBJECTS_HPP
#define LOVE_VULKAN_OBJECTS_HPP
#include <love/client/love_client.hpp>

#include <love/client/graphics/vulkan/command_pool.hpp>
#include <love/client/graphics/vulkan/frame_buffers.hpp>
#include <love/client/graphics/vulkan/graphics_device.hpp>
#include <love/client/graphics/vulkan/graphics_pipeline.hpp>
#include <love/client/graphics/vulkan/image_views.hpp>
#include <love/client/graphics/vulkan/render_pass.hpp>
#include <love/client/graphics/vulkan/swap_chain.hpp>
#include <love/client/graphics/window/window.hpp>

#include <love/common/data/files/logger.hpp>

#include <memory>

namespace love_engine {
    class LV_ENG_CLI VulkanObjects {
        public:
            struct Properties {
                Window::Properties windowProperties{};
                GraphicsDevice::Properties graphicsDeviceProperties{};
                SwapChain::Properties swapChainProperties{};
                ImageViews::Properties imageViewProperties{};
                RenderPass::Properties renderPassProperties{};
                GraphicsPipeline::Properties graphicsPipelineProperties{};
                FrameBuffers::Properties frameBufferProperties{};
                CommandPool::Properties commandPoolProperties{};

                std::vector<VkCommandBufferBeginInfo> commandBufferBeginInfo;
                std::vector<VkRenderPassBeginInfo> renderPassBeginInfo;
            };

            VulkanObjects(VkInstance instance, const Properties& properties, std::shared_ptr<Logger> logger);
            ~VulkanObjects();

            void createVulkanObjects() noexcept;

            inline Window* window() const noexcept { return _window.get(); }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkInstance _vulkanInstance = nullptr;

            std::unique_ptr<Window> _window = nullptr;
            std::unique_ptr<GraphicsDevice> _graphicsDevice = nullptr;
            std::unique_ptr<SwapChain> _swapChain = nullptr;
            std::unique_ptr<ImageViews> _imageViews = nullptr;
            std::unique_ptr<RenderPass> _renderPass = nullptr;
            std::unique_ptr<GraphicsPipeline> _graphicsPipeline = nullptr;
            std::unique_ptr<FrameBuffers> _frameBuffers = nullptr;
            std::unique_ptr<CommandPool> _commandPool = nullptr;

            void _log(const std::string& message) const noexcept;
    };
}

#endif // LOVE_VULKAN_OBJECTS_HPP
