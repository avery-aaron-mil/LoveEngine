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
            struct Properties {
                std::shared_ptr<VkSwapchainCreateInfoKHR> swapChainInfo = nullptr;
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
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~SwapChain();

            static SwapChainSupportDetails querySwapChainSupport(
                const VkPhysicalDevice& device,
                const VkSurfaceKHR& surface
            ) noexcept;

            inline const std::vector<VkImage>* swapChainImages() const noexcept { return &_swapChainImages; }
            inline VkFormat imageFormat() const noexcept { return _surfaceFormat.format; }

        private:
            std::shared_ptr<Logger> _logger;
            GraphicsDevice _graphicsDevice;
            Window _window;
            Properties _properties;
            VkDevice _device = nullptr;
            VkSwapchainKHR _swapChain = nullptr;
            std::vector<VkImage> _swapChainImages;

            VkSurfaceFormatKHR _surfaceFormat;
            VkPresentModeKHR _presentMode;
            VkExtent2D _extent;

            void _log(const std::string& message) const noexcept;
            void _initSwapChain(const GraphicsDevice& graphicsDevice) noexcept;
            void _createSwapChain() noexcept;
            static VkSurfaceFormatKHR _chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats) noexcept;
            VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const noexcept;
            VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const noexcept;
    };
}

#endif // LOVE_SWAP_CHAIN_HPP
