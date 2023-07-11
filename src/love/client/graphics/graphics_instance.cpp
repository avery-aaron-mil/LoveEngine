#include "graphics_instance.hpp"

#include <love/common/error/crash.hpp>
#include <love/common/love_engine_instance.hpp>

#include <cstring>
#include <sstream>

#include <GLFW/glfw3.h>

#include "vulkan_functions.hpp"

namespace love_engine {
    std::shared_ptr<Logger> _debugLogger;

    GraphicsInstance::GraphicsInstance(
        const ApplicationInfo& applicationInfo,
        const std::function<void(int, const char*)>& glfwErrorCallback,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _applicationInfo(applicationInfo) {
        _debugLogger = applicationInfo.debugLogger;
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

    void GraphicsInstance::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    static VkBool32 _logVulkan(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        if (_debugLogger.get() != nullptr) {
            std::stringstream buffer;
            buffer << "(Vulkan): " << pCallbackData->pMessage;
            switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                    _debugLogger.get()->log(LogStatus::VERBOSE, buffer.str());
                } break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                    _debugLogger.get()->log(LogStatus::INFO, buffer.str());
                } break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                    _debugLogger.get()->log(LogStatus::ERROR_, buffer.str());
                } break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                    _debugLogger.get()->log(LogStatus::WARNING, buffer.str());
                } break;
                default: {
                    _debugLogger.get()->log(LogStatus::MESSAGE, buffer.str());
                } break;
            }
        }
        return VK_FALSE;
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
        #undef VK_GLOBAL_LEVEL_FUNCTION

        _log("Loaded global Vulkan functions.");
    }

    void GraphicsInstance::_loadInstanceVulkanFunctions() const noexcept {
        _log("Loading Vulkan instance functions...");

        #define VK_INSTANCE_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetInstanceProcAddr(_vulkanInstance, #fun))) {\
            Crash::crash("Could not load Vulkan instance function: " #fun);\
        }
        #include "vulkan_functions.inl"
        #undef VK_INSTANCE_LEVEL_FUNCTION

        _log("Loaded Vulkan instance functions.");
    }

    void GraphicsInstance::_checkValidationLayerSupport(const std::vector<const char*>& layers) noexcept {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Get available layers
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        // Search for layers
        for (const auto& layer : layers) {
            bool layerFound = false;

            for (size_t i = 0; i < availableLayers.size(); i++) {
                if (std::strcmp(layer, availableLayers[i].layerName) == 0) {
                    layerFound = true;
                    availableLayers.erase(availableLayers.begin() + i, availableLayers.end());
                    break;
                }
            }

            if (!layerFound) {
                std::stringstream buffer;
                buffer << "Vulkan validation layer not found: " << layer;
                Crash::crash(buffer.str());
            }
        }
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

        // Create info
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
        VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &vulkanApplicationInfo,
            .enabledExtensionCount = count,
            .ppEnabledExtensionNames = extensions
        };

        // Validation layers
        std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        _checkValidationLayerSupport(validationLayers);
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        if (_debugLogger.get() != nullptr) {
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
            debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
            debugCreateInfo.pfnUserCallback = _logVulkan;

            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.pNext = &debugCreateInfo;
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &_vulkanInstance) != VK_SUCCESS) {
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
}
