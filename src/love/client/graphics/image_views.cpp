#include "image_views.hpp"

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    ImageViews::ImageViews(
        VkDevice device,
        const std::vector<VkImage>* swapChainImages,
        VkFormat imageFormat,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _device(device), _swapChainImages(swapChainImages), _imageFormat(imageFormat) {
        if (_device == nullptr) Crash::crash("Vulkan device passed to image views was null.");
        if (_swapChainImages == nullptr) Crash::crash("Swap chain images pointer passed to image views was null.");

        _createImageViews();
    }
    ImageViews::~ImageViews() {
        for (auto imageView : _swapChainImageViews) vkDestroyImageView(_device, imageView, nullptr);
    }

    void ImageViews::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void ImageViews::_createImageViews() noexcept {
        _log("Creating image views...");

        const size_t swapChainImageCount = _swapChainImages->size();
        _swapChainImageViews.resize(swapChainImageCount);
        for (size_t i = 0; i < swapChainImageCount; ++i) {
            VkImageViewCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = _swapChainImages->at(i),
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = _imageFormat
            };
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            
            if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
                Crash::crash("Failed to create image view.");
            }
        }

        _log("Created image views.");
    }
}
