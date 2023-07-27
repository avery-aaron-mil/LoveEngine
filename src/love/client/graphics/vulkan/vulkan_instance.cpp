#include <love/client/graphics/vulkan/vulkan_instance.hpp>

#include <love/client/graphics/vulkan/vulkan_functions.hpp>
#include <love/common/error/crash.hpp>
#include <love/common/love_engine_instance.hpp>

#include <cstring>
#include <sstream>

#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

namespace love_engine {
    static std::shared_ptr<Logger> _debugLogger;

    VulkanInstance::VulkanInstance(
        const ApplicationInfo& applicationInfo,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _applicationInfo(applicationInfo), _properties(properties) {
        _debugLogger = applicationInfo.debugLogger;
        _loadVulkanLibrary();
        _initializeGLFW();
        _loadGlobalVulkanFunctions();
        _createVulkanInstance();
        _loadInstanceVulkanFunctions();

        _vulkanObjects = std::make_unique<VulkanObjects>(_vulkanInstance, _properties.vulkanProperties, _logger);
        if (_vulkanObjects.get() == nullptr) Crash::crash("Failed to create VulkanObjects pointer.");
        _vulkanObjects.get()->createVulkanObjects();
    }
    
    VulkanInstance::~VulkanInstance() {
        if (_vulkanObjects.get()) _vulkanObjects.reset();
        if (_vulkanInstance) vkDestroyInstance(_vulkanInstance, nullptr);
        _log("Destroyed Vulkan instance.");

        glfwTerminate();
        _log("Terminated GLFW.");
    }

    void VulkanInstance::_defaultGLFWErrorCallback(int error, const char* description) {
        Crash::crash(std::string("Caught GLFW error with no callback: ") + description);
    }

    void VulkanInstance::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/VulkanInstance): " + message);
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

    void VulkanInstance::_loadVulkanLibrary() noexcept {
        _log("Loading Vulkan library...");
        _vulkanLibrary.loadLibrary("vulkan-1.dll");
        _log("Loaded Vulkan library.");
    }

    void VulkanInstance::_initializeGLFW() const noexcept {
        _log("Initializing GLFW...");
        if (!glfwInit()) {
            Crash::crash("Failed to initialize GLFW.");
        }
        glfwSetErrorCallback(*(_properties.glfwErrorCallback.target<void(*)(int, const char*)>()));

        if (!glfwVulkanSupported()) {
            Crash::crash("Vulkan not supported.");
        }

        _log("Initialized GLFW.");
    }

    void VulkanInstance::_loadGlobalVulkanFunctions() const noexcept {
        _log("Loading global Vulkan functions...");

        #define VK_EXPORTED_FUNCTION(fun) (fun = (PFN_##fun) _vulkanLibrary.loadLibraryFunction(#fun));
        #define VK_GLOBAL_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetInstanceProcAddr(nullptr, #fun))) {\
            Crash::crash("Could not load global Vulkan function: " #fun);\
        }
        #include <love/client/graphics/vulkan/vulkan_functions.inl>
        #undef VK_GLOBAL_LEVEL_FUNCTION

        _log("Loaded global Vulkan functions.");
    }

    

    void VulkanInstance::_createVulkanInstance() noexcept {
        _log("Creating Vulkan instance...");
        
        uint32_t apiVersion;
        auto result = vkEnumerateInstanceVersion(&apiVersion);
        if (result != VK_SUCCESS) {
            std::stringstream buffer;
            buffer << "Failed to get Vulkan API version: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        // Extensions
        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::stringstream buffer;
        buffer << "Required GLFW extensions:";
        for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
            _enabledExtensions.emplace_back(glfwExtensions[i]);
            buffer << "\n\t" << glfwExtensions[i];
        }
        _log(buffer.str());
        _validateEnabledExtensions();

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
            .enabledExtensionCount = static_cast<uint32_t>(_enabledExtensions.size()),
            .ppEnabledExtensionNames = _enabledExtensions.data()
        };

        // Validation layers
        _checkValidationLayerSupport(_validationLayers);
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        if (_debugLogger.get() != nullptr) {
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            if (_applicationInfo.verbose) {
                debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                ;
            } else {
                debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                ;
            }
            debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            ;
            debugCreateInfo.pfnUserCallback = _logVulkan;

            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = _validationLayers.data();
            instanceCreateInfo.pNext = &debugCreateInfo;
        }

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &_vulkanInstance);
        if (result != VK_SUCCESS) {
            buffer.str("");
            buffer << "Failed to create Vulkan instance: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        _log("Created Vulkan instance.");
    }

    void VulkanInstance::_checkValidationLayerSupport(const std::vector<const char*>& layers) const noexcept {
        _log("Checking validation layer support...");

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
                    break;
                }
            }

            if (!layerFound) {
                std::stringstream buffer;
                buffer << "Vulkan validation layer not found: " << layer;
                Crash::crash(buffer.str());
            }
        }

        _log("Checked validation layer support.");
    }

    void VulkanInstance::_validateEnabledExtensions() noexcept {
        _log("Validating extensions...");

        uint32_t extensionCount;
        auto result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream buffer;
            buffer << "Could not enumerate Vulkan instance extension properites: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        // Get available extensions
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        if (result != VK_SUCCESS) {
            std::stringstream buffer;
            buffer << "Could not enumerate Vulkan instance extension properites: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        // Search extensions
        for (const auto& extension : _enabledExtensions) {
            bool extensionFound = false;

            for (size_t i = 0; i < availableExtensions.size(); i++) {
                if (std::strcmp(extension, availableExtensions[i].extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound) {
                std::stringstream buffer;
                buffer << "Could not find extension \"" << extension << "\" for Vulkan instance.";
                Crash::crash(buffer.str());
            }
        }

        _log("Validated extensions.");
    }

    void VulkanInstance::_loadInstanceVulkanFunctions() const noexcept {
        _log("Loading Vulkan instance functions...");

        #define VK_INSTANCE_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetInstanceProcAddr(_vulkanInstance, #fun))) {\
            Crash::crash("Could not load Vulkan instance function: " #fun);\
        }
        #include <love/client/graphics/vulkan/vulkan_functions.inl>
        #undef VK_INSTANCE_LEVEL_FUNCTION

        _log("Loaded Vulkan instance functions.");
    }
}
