#include "swap_chain.hpp"

#include <love/common/error/crash.hpp>

namespace love_engine {
    SwapChain::SwapChain(const GraphicsDevice::SwapChainSupportDetails& swapChainSupport, const GraphicsDevice::Settings& settings)
    : _settings(settings) {
        _surfaceFormat = _chooseSwapChainFormat(swapChainSupport.surfaceFormats);
        _presentMode = _chooseSwapPresentMode(swapChainSupport.presentModes);
        // TODO Change present mode mid-way
    }

    SwapChain::~SwapChain() {

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
