#include <love/client/graphics/vulkan/image_views.hpp>

#include <sstream>

#include <love/client/graphics/vulkan/vulkan_functions.hpp>
#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

namespace love_engine {
    ImageViews::ImageViews(
        VkDevice device,
        const std::vector<VkImage>* swapChainImages,
        VkFormat imageFormat,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _device(device), _swapChainImages(swapChainImages), _imageFormat(imageFormat), _properties(properties) {
        if (_device == nullptr) Crash::crash("Vulkan device passed to image views was null.");
        if (_swapChainImages == nullptr) Crash::crash("Swap chain images pointer passed to image views was null.");

        _createImageViews();
    }
    ImageViews::~ImageViews() {
        for (auto imageView : _swapChainImageViews) vkDestroyImageView(_device, imageView, nullptr);
    }

    void ImageViews::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/ImageViews): " + message);
        }
    }

    void ImageViews::_createImageViews() noexcept {
        _log("Creating image views...");

        if (_properties.imageViewInfoTemplate.get() == nullptr) {
            VkImageViewCreateInfo imageViewInfoTemplate {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = _imageFormat
            };
            imageViewInfoTemplate.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfoTemplate.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfoTemplate.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfoTemplate.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfoTemplate.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfoTemplate.subresourceRange.baseMipLevel = 0;
            imageViewInfoTemplate.subresourceRange.levelCount = 1;
            imageViewInfoTemplate.subresourceRange.baseArrayLayer = 0;
            imageViewInfoTemplate.subresourceRange.layerCount = 1;

            _properties.imageViewInfoTemplate = std::make_shared<VkImageViewCreateInfo>(imageViewInfoTemplate);
        }

        const size_t swapChainImageCount = _swapChainImages->size();
        _swapChainImageViews.resize(swapChainImageCount);
        for (size_t i = 0; i < swapChainImageCount; ++i) {
            VkImageViewCreateInfo createInfo = *(_properties.imageViewInfoTemplate.get());
            createInfo.image = _swapChainImages->at(i);

            auto result = vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]);
            if (result != VK_SUCCESS) {
                std::stringstream crashBuffer;
                crashBuffer << "Failed to create image view: " << string_VkResult(result);
                Crash::crash(crashBuffer.str());
            }
        }

        _log("Created image views.");
    }
}
