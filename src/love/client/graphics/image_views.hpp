#ifndef LOVE_IMAGE_VIEWS_HPP
#define LOVE_IMAGE_VIEWS_HPP

#include <memory>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class ImageViews {
        public:
            struct Properties {
                std::shared_ptr<VkImageViewCreateInfo> imageViewInfoTemplate = nullptr;
            };

            ImageViews(
                VkDevice device,
                const std::vector<VkImage>* swapChainImages,
                VkFormat imageFormat,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~ImageViews();

        private:
            std::shared_ptr<Logger> _logger;
            VkDevice _device = nullptr;
            const std::vector<VkImage>* _swapChainImages = nullptr;
            VkFormat _imageFormat;
            Properties _properties;
            std::vector<VkImageView> _swapChainImageViews;

            void _log(const std::string& message) const noexcept;
            void _createImageViews() noexcept;
    };
}

#endif // LOVE_IMAGE_VIEWS_HPP
