#ifndef LOVE_GRAPHICS_PIPELINE_HPP
#define LOVE_GRAPHICS_PIPELINE_HPP

#include <memory>
#include <string>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class GraphicsPipeline {
        public:
            GraphicsPipeline(VkDevice device, std::shared_ptr<Logger> logger);
            ~GraphicsPipeline();

        private:
            std::shared_ptr<Logger> _logger;
            VkDevice _device;
    };
}

#endif // LOVE_GRAPHICS_PIPELINE_HPP
