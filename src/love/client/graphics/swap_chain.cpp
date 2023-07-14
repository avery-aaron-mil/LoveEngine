#include "swap_chain.hpp"

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    SwapChain::SwapChain(
        const GraphicsDevice& graphicsDevice,
        const Window& window,
        const Settings& settings,
        std::shared_ptr<Logger> logger)
    : _logger(logger), _settings(settings), _window(window) {
        _initSwapChain(graphicsDevice);
    }
    SwapChain::~SwapChain() {

    }

    void SwapChain::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void SwapChain::_initSwapChain(const GraphicsDevice& graphicsDevice) noexcept {
        _log("Initializing swap chain...");

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(graphicsDevice.physicalDevice(), _window.surface());
        _surfaceFormat = _chooseSwapChainFormat(swapChainSupport.surfaceFormats);
        _presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes);
        vkGetDeviceQueue(graphicsDevice.device(), graphicsDevice.queueFamilyIndices().graphicsQueue, 0, &_graphicsQueue);
        vkGetDeviceQueue(graphicsDevice.device(), graphicsDevice.queueFamilyIndices().presentQueue, 0, &_presentQueue);
        // TODO Change present mode mid-way

        _log("Initialized swap chain.");
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

}
