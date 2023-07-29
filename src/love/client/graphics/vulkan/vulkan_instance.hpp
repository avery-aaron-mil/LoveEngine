#ifndef LOVE_VULKAN_INSTANCE_HPP
#define LOVE_VULKAN_INSTANCE_HPP
#include <love/client/love_client.hpp>

#include <love/client/graphics/vulkan/vulkan_objects.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/library.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <vulkan/vulkan.h> // Must be included before GLFW
#if defined (_WIN32)
  #ifndef VK_USE_PLATFORM_WIN32_KHR
  #define VK_USE_PLATFORM_WIN32_KHR
  #endif
  #include <windows.h>
  #include <vulkan/vulkan_win32.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  #ifndef VK_USE_PLATFORM_XCB_KHR
  #define VK_USE_PLATFORM_XCB_KHR
  #endif
  #include <xbc/proto.h>
  #include <vulkan/vulkan_xcb.h>
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  #ifndef VK_USE_PLATFORM_XLIB_KHR
  #define VK_USE_PLATFORM_XLIB_KHR
  #endif
  #include <X11/Xlib.h>
  #include <vulkan/vulkan_xlib.h>
#elif defined(__ANDROID__)
  #ifndef VK_USE_PLATFORM_ANDROID_KHR
  #define VK_USE_PLATFORM_ANDROID_KHR
  #endif
  #include <vulkan/vulkan_android.h>
#endif

namespace love_engine {
    class LV_ENG_CLI VulkanInstance {
        public:
            struct ApplicationInfo {
                std::string name;
                uint8_t versionMajor = 0;
                uint8_t versionMinor = 0;
                uint8_t versionPatch = 0;
                std::shared_ptr<Logger> debugLogger;
                bool verbose = false;
            };
            struct Properties {
                VulkanObjects::Properties vulkanProperties;

                std::function<void(int, const char*)> glfwErrorCallback = _defaultGLFWErrorCallback;

                std::vector<VkCommandBufferBeginInfo> commandBufferBeginInfo;
                std::vector<VkRenderPassBeginInfo> renderPassBeginInfo;
            };

            VulkanInstance(
                const ApplicationInfo& applicationInfo,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~VulkanInstance();

            inline VkInstance instance() const noexcept { return _vulkanInstance; }
            inline Window* window() const noexcept { return _vulkanObjects.get()->window(); }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            ApplicationInfo _applicationInfo;
            Properties _properties;
            Library _vulkanLibrary;
            VkInstance _vulkanInstance = nullptr;
            std::unique_ptr<VulkanObjects> _vulkanObjects = nullptr;

            std::vector<const char*> _enabledExtensions = {
                VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
                VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
                VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
                VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#endif
            };
            const std::vector<const char *> _validationLayers = { "VK_LAYER_KHRONOS_validation" };

            void _log(const std::string& message) const noexcept;
            void _loadVulkanLibrary() noexcept;
            void _loadGlobalVulkanFunctions() const noexcept;
            void _loadInstanceVulkanFunctions() const noexcept;
            void _checkValidationLayerSupport(const std::vector<const char*>& layers) const noexcept;
            void _validateEnabledExtensions() noexcept;
            void _createVulkanInstance() noexcept;
            void _initializeGLFW() const noexcept;

            static void _defaultGLFWErrorCallback(int error, const char* description);
    };
}

#endif // LOVE_VULKAN_INSTANCE_HPP
