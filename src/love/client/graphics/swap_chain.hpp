#ifndef LOVE_SWAP_CHAIN_HPP
#define LOVE_SWAP_CHAIN_HPP

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "graphics_device.hpp"
#include "window.hpp"

namespace love_engine {
    class SwapChain {
        public:
            struct Settings {
                VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            };

            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> surfaceFormats;
                std::vector<VkPresentModeKHR> presentModes;
            };

            SwapChain(
                const GraphicsDevice& graphicsDevice,
                const Window& window,
                const Settings& settings,
                std::shared_ptr<Logger> logger
            );
            ~SwapChain();

            static SwapChainSupportDetails querySwapChainSupport(
                const VkPhysicalDevice& device,
                const VkSurfaceKHR& surface
            ) noexcept;

        private:
            std::shared_ptr<Logger> _logger;
            GraphicsDevice _graphicsDevice;
            Window _window;
            Settings _settings;
            VkSwapchainKHR _swapChain = nullptr;
            VkQueue _graphicsQueue = nullptr;
            VkQueue _presentQueue = nullptr;

            void _log(const std::string& message) const noexcept;
            void _initSwapChain(const GraphicsDevice& graphicsDevice) noexcept;
            void _createSwapChain() noexcept;
            static VkSurfaceFormatKHR _chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept;
            VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const noexcept;
            VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept;
    };
}

#endif // LOVE_SWAP_CHAIN_HPP
