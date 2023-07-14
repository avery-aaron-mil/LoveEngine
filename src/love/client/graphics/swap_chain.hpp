#ifndef LOVE_SWAP_CHAIN_HPP
#define LOVE_SWAP_CHAIN_HPP

#include "graphics_device.hpp"

#include <vector>

#include <vulkan/vulkan.h>

namespace love_engine {
    class SwapChain {
        public:
            SwapChain(const GraphicsDevice::SwapChainSupportDetails& swapChainSupport, const GraphicsDevice::Settings& settings);
            ~SwapChain();

        private:
            GraphicsDevice::Settings _settings;
            VkSurfaceFormatKHR _surfaceFormat;
            VkPresentModeKHR _presentMode;

            static VkSurfaceFormatKHR _chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept;
            VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const noexcept;
    };
}

#endif // LOVE_SWAP_CHAIN_HPP
