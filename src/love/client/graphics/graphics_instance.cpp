#include "graphics_instance.hpp"

#include <love/common/error/crash.hpp>
#include <love/common/love_engine_instance.hpp>

#include <GLFW/glfw3.h>

#include "vulkan_functions.hpp"

namespace love_engine {
    void GraphicsInstance::_log(const std::string& message) const noexcept {
        if (_applicationInfo.logger.get() != nullptr) {
            _applicationInfo.logger.get()->log(message);
        }
    }
    
    void GraphicsInstance::_loadVulkanLibrary() noexcept {
        _log("Loading Vulkan library...");
        _vulkanLibrary.loadLibrary("vulkan-1.dll");
        _log("Loaded Vulkan library.");
    }

    void GraphicsInstance::_loadGlobalVulkanFunctions() const noexcept {
        _log("Loading global Vulkan functions...");
        #define VK_EXPORTED_FUNCTION(fun) (fun = (PFN_##fun) _vulkanLibrary.loadLibraryFunction(#fun));
        #define VK_GLOBAL_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetInstanceProcAddr(nullptr, #fun))) {\
            Crash::crash("Could not load global Vulkan function: " #fun);\
        }
        #include "vulkan_functions.inl"
        _log("Loaded global Vulkan functions.");
    }

    void GraphicsInstance::_loadInstanceVulkanFunctions() const noexcept {
        _log("Loading Vulkan instance functions...");
        #define VK_GLOBAL_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetInstanceProcAddr(_vulkanInstance, #fun))) {\
            Crash::crash("Could not load Vulkan instance function: " #fun);\
        }
        #include "vulkan_functions.inl"
        _log("Loaded Vulkan instance functions.");
    }

    void GraphicsInstance::_createVulkanInstance() noexcept {
        _log("Creating Vulkan instance...");
        uint32_t apiVersion;
        if (vkEnumerateInstanceVersion(&apiVersion) != VK_SUCCESS) {
            Crash::crash("Failed to get Vulkan API version.");
        }
        
        // GLFW Extensions
        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        VkApplicationInfo vulkanApplicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = _applicationInfo.name.c_str(),
            .applicationVersion = VK_MAKE_API_VERSION(
                1,
                _applicationInfo.versionMajor,
                _applicationInfo.versionMinor,
                _applicationInfo.versionPatch
            ),
            .pEngineName = "Love Engine",
            .engineVersion = VK_MAKE_API_VERSION(
                1,
                LoveEngineInstance::LOVE_ENGINE_VERSION_MAJOR,
                LoveEngineInstance::LOVE_ENGINE_VERSION_MINOR,
                LoveEngineInstance::LOVE_ENGINE_VERSION_PATCH
            ),
            .apiVersion = apiVersion
        };

        VkInstanceCreateInfo instanceInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &vulkanApplicationInfo,
            .enabledExtensionCount = count,
            .ppEnabledExtensionNames = extensions
        };

        if (vkCreateInstance(&instanceInfo, nullptr, &_vulkanInstance) != VK_SUCCESS) {
            Crash::crash("Failed to create Vulkan instance.");
        }

        _log("Created Vulkan instance.");
    }

    void GraphicsInstance::_initializeGLFW(const std::function<void(int, const char*)>& glfwErrorCallback) const noexcept {
        _log("Initializing GLFW...");
        if (!glfwInit()) {
            Crash::crash("Failed to initialize GLFW.");
        }
        glfwSetErrorCallback(*(glfwErrorCallback.target<void(*)(int, const char*)>()));

        if (!glfwVulkanSupported()) {
            Crash::crash("Vulkan not supported.");
        }
        
        _log("Initialized GLFW.");
    }

    GraphicsInstance::GraphicsInstance(
        const ApplicationInfo& applicationInfo,
        const std::function<void(int, const char*)>& glfwErrorCallback
    ) : _applicationInfo(applicationInfo) {
        _loadVulkanLibrary();
        _initializeGLFW(glfwErrorCallback);
        _loadGlobalVulkanFunctions();
        _createVulkanInstance();
        _loadInstanceVulkanFunctions();
    }
    
    GraphicsInstance::~GraphicsInstance() {
        glfwTerminate();
        if (_vulkanInstance) vkDestroyInstance(_vulkanInstance, nullptr);
    }
}
