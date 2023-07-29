#ifndef LOVE_FRAME_BUFFERS_HPP
#define LOVE_FRAME_BUFFERS_HPP
#include <love/client/love_client.hpp>

#include <memory>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class LV_ENG_CLI FrameBuffers {
        public:
            struct Properties {
                std::vector<VkFramebufferCreateInfo> frameBufferInfo;
            };

            FrameBuffers(
                VkDevice device,
                VkRenderPass renderPass,
                const VkExtent2D& extent,
                const std::vector<VkImageView>& imageViews,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~FrameBuffers();

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkDevice _device = nullptr;
            VkRenderPass _renderPass = nullptr;
            VkExtent2D _extent;
            std::vector<VkFramebuffer> _swapChainFrameBuffers;

            void _log(const std::string& message) const noexcept;
            void _createFrameBuffers(const std::vector<VkImageView>& imageViews) noexcept;
    };
}

#endif // LOVE_FRAME_BUFFERS_HPP
