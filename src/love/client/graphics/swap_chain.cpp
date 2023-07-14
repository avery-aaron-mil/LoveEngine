#include "swap_chain.hpp"

#include <algorithm>

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    SwapChain::SwapChain(
        const GraphicsDevice& graphicsDevice,
        const Window& window,
        const Settings& settings,
        std::shared_ptr<Logger> logger)
    : _logger(logger), _graphicsDevice(graphicsDevice), _window(window), _settings(settings) {
        _createSwapChain();
    }
    SwapChain::~SwapChain() {
        if (_swapChain) vkDestroySwapchainKHR(_graphicsDevice.device(), _swapChain, nullptr);
    }

    void SwapChain::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void SwapChain::_createSwapChain() noexcept {
        _log("Creating image swap chain...");

        // Get data for create info
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_graphicsDevice.physicalDevice(), _window.surface());
        VkSurfaceFormatKHR surfaceFormat = _chooseSwapChainFormat(swapChainSupport.surfaceFormats);
        VkPresentModeKHR presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = _chooseSwapExtent(swapChainSupport.capabilities);
        const uint32_t graphicsQueueIndex = _graphicsDevice.queueFamilyIndices().graphicsQueue;
        const uint32_t presentQueueIndex = _graphicsDevice.queueFamilyIndices().presentQueue;
        const uint32_t queueFamilyIndices[] = {graphicsQueueIndex, presentQueueIndex};
        vkGetDeviceQueue(_graphicsDevice.device(), graphicsQueueIndex, 0, &_graphicsQueue);
        vkGetDeviceQueue(_graphicsDevice.device(), presentQueueIndex, 0, &_presentQueue);

        // Set image count
        // NOTE: Use minimum + 1 for avoid needing to wait for driver to complete operations
        //       before we can get another image to render to.
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if ((swapChainSupport.capabilities.maxImageCount > 0) && // 0 means no maximum
            (imageCount > swapChainSupport.capabilities.maxImageCount)
        ) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = _window.surface(),
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1, // Layers of images to render to, always 1 unless using a stereoscopic application for VR/AR
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing and HUDs
            .preTransform = swapChainSupport.capabilities.currentTransform, // Use currentt transform without any operations
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // No alpha channel unless transparent window
            .presentMode = presentMode,
            .clipped = VK_TRUE // Clip pixels behind window
        };

        if (graphicsQueueIndex != presentQueueIndex) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateSwapchainKHR(_graphicsDevice.device(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
            Crash::crash("Failed to create swap chain.");
        }

        _log("Created image swap chain.");
    }

    SwapChain::SwapChainSupportDetails SwapChain::querySwapChainSupport(
        const VkPhysicalDevice& device,
        const VkSurfaceKHR& surface
    ) noexcept {
        SwapChainSupportDetails details;
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities) != VK_SUCCESS) {
            Crash::crash("Could not get surface capabilities for physical graphics device.");
        }

        uint32_t formatCount;
        if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr) != VK_SUCCESS) {
            Crash::crash("Could not get surface formats for physical graphics device.");
        }

        if (formatCount > 0) {
            details.surfaceFormats.resize(formatCount);
            if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormats.data()) != VK_SUCCESS) {
                Crash::crash("Could not get surface formats for physical graphics device.");
            }
        }

        uint32_t presentModeCount;
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr) != VK_SUCCESS) {
            Crash::crash("Could not get surface present modes for physical graphics device.");
        }

        if (presentModeCount > 0) {
            details.presentModes.resize(presentModeCount);
            if (vkGetPhysicalDeviceSurfacePresentModesKHR(
                    device,
                    surface,
                    &presentModeCount,
                    details.presentModes.data()
                ) != VK_SUCCESS
            ) Crash::crash("Could not get surface present modes for physical graphics device.");
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
            if (presentMode == _settings.preferredPresentMode) {
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