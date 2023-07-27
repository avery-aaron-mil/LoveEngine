#ifndef LOVE_RENDER_PASS_HPP
#define LOVE_RENDER_PASS_HPP

#include <memory>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class RenderPass {
        public:
            struct Properties {
                std::shared_ptr<VkRenderPassCreateInfo> renderPassInfo = nullptr;
            };

            RenderPass(VkDevice device, VkFormat imageFormat, const Properties& properties, std::shared_ptr<Logger> logger);
            ~RenderPass();

            VkRenderPass renderPass() const noexcept { return _renderPass; }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkDevice _device = nullptr;
            VkFormat _imageFormat;
            VkRenderPass _renderPass = nullptr;

            void _log(const std::string& message) const noexcept;
            void _createRenderPass() noexcept;
    };
}

#endif // LOVE_RENDER_PASS_HPP
