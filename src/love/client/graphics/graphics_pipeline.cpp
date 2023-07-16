#include "graphics_pipeline.hpp"

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    GraphicsPipeline::GraphicsPipeline(VkDevice device, std::shared_ptr<Logger> logger)
    : _logger(logger), _device(device) {
        
    }
    GraphicsPipeline::~GraphicsPipeline() {

    }
}
