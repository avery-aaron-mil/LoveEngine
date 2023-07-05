#include "graphics_device.hpp"

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    void GraphicsDevice::_log(const std::string& message) const noexcept {
        if (_settings.logger.get() != nullptr) {
            _settings.logger.get()->log(message);
        }
    }

    void GraphicsDevice::_load_Device_Vulkan_Functions() const noexcept {
        _log("Loading Vulkan device functions...");
#define VK_GLOBAL_LEVEL_FUNCTION(fun)\
        if(!(fun = (PFN_##fun) vkGetDeviceProcAddr(_device, #fun))) {\
            Crash::crash("Could not load Vulkan device function: " #fun);\
        }
#include "vulkan_functions.inl"
        _log("Loaded Vulkan device functions.");
    }

    GraphicsDevice::GraphicsDevice() {
        // TODO
    }

    GraphicsDevice::~GraphicsDevice() {
        if (_device) vkDestroyDevice(_device, nullptr);
    }
}
