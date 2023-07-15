#ifndef LOVE_IMAGE_VIEWS_HPP
#define LOVE_IMAGE_VIEWS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "swap_chain.hpp"

namespace love_engine {
    class ImageViews {
        public:
            ImageViews(const SwapChain* swapChain);
            ~ImageViews();

        private:
            const SwapChain* _swapChain;
            VkDevice _device = nullptr;
            const std::vector<VkImage>* _swapChainImages = nullptr;
            std::vector<VkImageView> _swapChainImageViews;

            void _createImageViews() noexcept;
    };
}

#endif // LOVE_IMAGE_VIEWS_HPP
