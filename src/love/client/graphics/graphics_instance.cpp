#include <love/client/graphics/graphics_instance.hpp>

#include <love/common/error/crash.hpp>

namespace love_engine {
    GraphicsInstance::GraphicsInstance(const Properties& properties, std::shared_ptr<Logger> logger)
     : _logger(logger), _properties(properties) {
        _log("Initializing graphics...\n\n================= LOVE ENGINE GRAPHICS INITIALIZING =================\n");
        
        _vulkanInstance = std::make_unique<VulkanInstance>(
            _properties.applicationInfo,
            _properties.instanceProperties,
            _logger
        );
     }
    
    void GraphicsInstance::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/GraphicsInstance): " + message);
        }
    }
}
