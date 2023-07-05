#include "graphics_instance.hpp"

#include <love/common/error/crash.hpp>
#include <love/common/love_engine_instance.hpp>

#include <GLFW/glfw3.h>

namespace love_engine {
    void GraphicsInstance::_load_Global_Vulkan_Functions() noexcept {
#define VK_EXPORTED_FUNCTION(fun) (fun = (PFN_##fun) _vulkanLibrary.load_Library_Function(#fun))
#define VK_GLOBAL_LEVEL_FUNCTION(fun) (fun = (PFN_##fun) vkGetInstanceProcAddr(nullptr, #fun))
#include "vulkan_functions.inl"
    }

    void GraphicsInstance::_create_Vulkan_Instance(const ApplicationInfo& applicationInfo) noexcept {
        uint32_t apiVersion;
        if (vkEnumerateInstanceVersion(&apiVersion) != VK_SUCCESS) {
            Crash::crash("Failed to get Vulkan API version.");
        }
        
        // GLFW Extensions
        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        VkApplicationInfo vulkanApplicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = applicationInfo.name.c_str(),
            .applicationVersion = VK_MAKE_API_VERSION(
                1,
                applicationInfo.versionMajor,
                applicationInfo.versionMinor,
                applicationInfo.versionPatch
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
    }

    void GraphicsInstance::_initialize_GLFW(
        const ApplicationInfo& applicationInfo,
        const std::function<void(int, const char*)>& glfwErrorCallback
    ) noexcept {
        if (!glfwInit()) {
            Crash::crash("Failed to initialize GLFW.");
        }
        glfwSetErrorCallback(*(glfwErrorCallback.target<void(*)(int, const char*)>()));

        if (!glfwVulkanSupported()) {
            Crash::crash("Vulkan not supported.");
        }
    }

    GraphicsInstance::GraphicsInstance(
        const ApplicationInfo& applicationInfo,
        const std::function<void(int, const char*)>& glfwErrorCallback
    ) {
        _vulkanLibrary.load_Library( "vulkan-1.dll" );
        _initialize_GLFW(applicationInfo, glfwErrorCallback);
        _load_Global_Vulkan_Functions();
        _create_Vulkan_Instance(applicationInfo);
    }
    
    GraphicsInstance::~GraphicsInstance() {
        glfwTerminate();
        if (_vulkanInstance) vkDestroyInstance(_vulkanInstance, nullptr);
    }
}
