#include "graphics_instance.hpp"

#include <love/common/error/crash.hpp>

namespace love_engine {
    GraphicsInstance::GraphicsInstance(const Properties& properties, std::shared_ptr<Logger> logger)
     : _logger(logger), _properties(properties) {}
    
    void GraphicsInstance::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/GraphicsInstance): " + message);
        }
    }

    void GraphicsInstance::_defaultGLFWErrorCallback(int error, const char* description) {
        Crash::crash(std::string("Caught GLFW error with no callback: ") + description);
    }
}
