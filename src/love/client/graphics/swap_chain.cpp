#include "swap_chain.hpp"

#include <algorithm>
#include <sstream>

#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

#include "vulkan_functions.hpp"

namespace love_engine {
    SwapChain::SwapChain(
        const GraphicsDevice& graphicsDevice,
        const Window& window,
        const Properties& properties,
        std::shared_ptr<Logger> logger)
    : _logger(logger), _graphicsDevice(graphicsDevice), _window(window), _properties(properties) {
        _device = _graphicsDevice.device();
        if (_device == nullptr) Crash::crash("Vulkan device passed to swap chain was null.");

        _createSwapChain();
    }
    SwapChain::~SwapChain() {
        if (_swapChain) vkDestroySwapchainKHR(_device, _swapChain, nullptr);
    }

    void SwapChain::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/SwapChain): " + message);
        }
    }

    void SwapChain::_createSwapChain() noexcept {
        _log("Creating swap chain...");

        // Get data for create info
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_graphicsDevice.physicalDevice(), _window.surface());
        _surfaceFormat = _chooseSwapChainFormat(swapChainSupport.surfaceFormats);
        _presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes);
        _extent = _chooseSwapExtent(swapChainSupport.capabilities);
        const uint32_t graphicsQueueIndex = _graphicsDevice.queueFamilyIndices().graphicsQueue;
        const uint32_t presentQueueIndex = _graphicsDevice.queueFamilyIndices().presentQueue;
        const uint32_t queueFamilyIndices[] = {graphicsQueueIndex, presentQueueIndex};

        // Set image count
        // NOTE: Use minimum + 1 for avoid needing to wait for driver to complete operations
        //       before we can get another image to render to.
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if ((swapChainSupport.capabilities.maxImageCount > 0) && // 0 means no maximum
            (imageCount > swapChainSupport.capabilities.maxImageCount)
        ) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        if (_properties.swapChainInfo.get() == nullptr) {
            VkSwapchainCreateInfoKHR createInfo {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = _window.surface(),
                .minImageCount = imageCount,
                .imageFormat = _surfaceFormat.format,
                .imageColorSpace = _surfaceFormat.colorSpace,
                .imageExtent = _extent,
                .imageArrayLayers = 1, // Layers of images to render to, always 1 unless using a stereoscopic application for VR/AR
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing and HUDs
                .preTransform = swapChainSupport.capabilities.currentTransform, // Use currentt transform without any operations
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // No alpha channel unless transparent window
                .presentMode = _presentMode,
                .clipped = VK_TRUE // Clip pixels behind window
            };
            _properties.swapChainInfo = std::make_shared<VkSwapchainCreateInfoKHR>(createInfo);
        }

        if (graphicsQueueIndex != presentQueueIndex) {
            _properties.swapChainInfo.get()->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            _properties.swapChainInfo.get()->queueFamilyIndexCount = 2;
            _properties.swapChainInfo.get()->pQueueFamilyIndices = queueFamilyIndices;
        } else _properties.swapChainInfo.get()->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = vkCreateSwapchainKHR(_device, _properties.swapChainInfo.get(), nullptr, &_swapChain);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to create swap chain: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        // Get swap chain images
        result = vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to get swap chain images: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }
        _swapChainImages.resize(imageCount);
        result = vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to get swap chain images: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        _log("Created swap chain.");
    }

    SwapChain::SwapChainSupportDetails SwapChain::querySwapChainSupport(
        const VkPhysicalDevice& device,
        const VkSurfaceKHR& surface
    ) noexcept {
        SwapChainSupportDetails details;
        auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to get surface capabilities for physical graphics device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        uint32_t formatCount;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to get surface formats for physical graphics device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        if (formatCount > 0) {
            details.surfaceFormats.resize(formatCount);
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormats.data());
            if (result != VK_SUCCESS) {
                std::stringstream crashBuffer;
                crashBuffer << "Failed to get surface formats for physical graphics device: " << string_VkResult(result);
                Crash::crash(crashBuffer.str());
            }
        }

        uint32_t presentModeCount;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to get surface present modes for physical graphics device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        if (presentModeCount > 0) {
            details.presentModes.resize(presentModeCount);
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
            if (result != VK_SUCCESS) {
                std::stringstream crashBuffer;
                crashBuffer << "Failed to get surface present modes for physical graphics device: " << string_VkResult(result);
                Crash::crash(crashBuffer.str());
            }
        }

        return details;
    }

    VkSurfaceFormatKHR SwapChain::_chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept {
        if (formats.empty()) {
            Crash::crash("Tried to choose swap chain format from an empty list of swap chain formats.");
        }

        for (const auto& availableFormat : formats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return formats[0];
    }

    VkPresentModeKHR SwapChain::_chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const noexcept {
        if (presentModes.empty()) {
            Crash::crash("Tried to choose swap present mode from an empty list of swap present modes.");
        }

        for (const auto& presentMode : presentModes) {
            if (presentMode == _properties.preferredPresentMode) {
                return presentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::_chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = _window.extent();
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}
