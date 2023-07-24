#include "frame_buffers.hpp"

#include <sstream>

#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

#include "vulkan_functions.hpp"

namespace love_engine {
    FrameBuffers::FrameBuffers(
        VkDevice device,
        VkRenderPass renderPass,
        const VkExtent2D& extent,
        const std::vector<VkImageView>& imageViews,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    )
    : _logger(logger), _properties(properties), _device(device), _renderPass(renderPass), _extent(extent) {
        if (_device == nullptr) Crash::crash("Device passed to swap chain frame buffers was null.");
        if (_renderPass == nullptr) Crash::crash("Render pass passed to swap chain frame buffers was null.");
        _createFrameBuffers(imageViews);
    }
    FrameBuffers::~FrameBuffers() {
        for (auto frameBuffer : _swapChainFrameBuffers) vkDestroyFramebuffer(_device, frameBuffer, nullptr);
    }

    void FrameBuffers::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/FrameBuffers): " + message);
        }
    }

    void FrameBuffers::_createFrameBuffers(const std::vector<VkImageView>& imageViews) noexcept {
        _log("Creating swap chain frame buffers...");

        _swapChainFrameBuffers.resize(imageViews.size());
        if (_properties.frameBufferInfo.empty()) {
            for (size_t i = 0; i < imageViews.size(); ++i) {
                VkFramebufferCreateInfo frameBufferInfo {
                    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                    .renderPass = _renderPass,
                    .attachmentCount = 1,
                    .pAttachments = &imageViews[i],
                    .width = _extent.width,
                    .height = _extent.height,
                    .layers = 1
                };

                _properties.frameBufferInfo.emplace_back(std::move(frameBufferInfo));
            }
        }

        VkResult result;
        for (size_t i = 0; i < imageViews.size(); ++i) {
            result = vkCreateFramebuffer(_device, &_properties.frameBufferInfo[i], nullptr, &_swapChainFrameBuffers[i]);

            if (result != VK_SUCCESS) {
                std::stringstream crashBuffer;
                crashBuffer << "Failed to create frame buffer: " << string_VkResult(result);
                Crash::crash(crashBuffer.str());
            }
        }

        _log("Created swap chain frame buffers.");
    }
}
