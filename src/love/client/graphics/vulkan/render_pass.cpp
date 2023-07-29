#include <love/client/graphics/vulkan/render_pass.hpp>

#include <sstream>

#include <love/client/graphics/vulkan/vulkan_functions.hpp>
#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

namespace love_engine {
    RenderPass::RenderPass(VkDevice device, VkFormat imageFormat, const Properties& properties, std::shared_ptr<Logger> logger)
     : _logger(logger), _properties(properties), _device(device), _imageFormat(imageFormat) {
        if (_device == nullptr) Crash::crash("Vulkan device to graphics pipeline was null.");
        _createRenderPass();
    }
    RenderPass::~RenderPass() {
        if (_renderPass) vkDestroyRenderPass(_device, _renderPass, nullptr);
        _log("Destroyed render pass.");
    }

    void RenderPass::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/RenderPass): " + message);
        }
    }

    void RenderPass::_createRenderPass() noexcept {
        _log("Creating render pass...");

        if (_properties.renderPassInfo.get() == nullptr) {
            VkAttachmentDescription colorAttachment {
                .format = _imageFormat,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            };

            VkAttachmentReference colorAttachmentReference {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            VkSubpassDescription subpass {
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachmentReference
            };

            VkRenderPassCreateInfo renderPassInfo {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &colorAttachment,
                .subpassCount = 1,
                .pSubpasses = &subpass
            };

            _properties.renderPassInfo = std::make_shared<VkRenderPassCreateInfo>(std::move(renderPassInfo));
        }

        auto result = vkCreateRenderPass(_device, _properties.renderPassInfo.get(), nullptr, &_renderPass);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to create render pass: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        _log("Created render pass.");
    }
}