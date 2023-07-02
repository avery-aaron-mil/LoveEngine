#include "graphics_instance.hpp"

#include <love/common/error/crash.hpp>
#include <love/common/love_engine_instance.hpp>

namespace love_engine {
    void GraphicsInstance::_initialize_Vulkan(
        const ApplicationInfo& applicationInfo,
        const char** extensions, uint32_t count
    ) noexcept {
        PFN_vkEnumerateInstanceVersion pfnEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)
            vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");

        uint32_t apiVersion;
        if (pfnEnumerateInstanceVersion == nullptr) {
            Crash::crash("Could not find vkEnumerateInstanceVersion().");
        }
        if (pfnEnumerateInstanceVersion(&apiVersion) != VK_SUCCESS) {
            Crash::crash("Failed to get Vulkan API version.");
        }

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

        PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)
            vkGetInstanceProcAddr(nullptr, "vkCreateInstance");
        if (pfnCreateInstance == nullptr) {
            Crash::crash("Could not find vkCreateInstance().");
        }
        if (pfnCreateInstance(&instanceInfo, nullptr, &_vulkanInstance) != VK_SUCCESS) {
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
        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);
        _initialize_Vulkan(applicationInfo, extensions, count);
    }

    GraphicsInstance::GraphicsInstance(
        const ApplicationInfo& applicationInfo,
        const std::function<void(int, const char*)>& glfwErrorCallback
    ) {
        _initialize_GLFW(applicationInfo, glfwErrorCallback);
    }
    
    GraphicsInstance::~GraphicsInstance() {
        glfwTerminate();
    }
}
