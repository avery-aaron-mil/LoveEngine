#ifndef LOVE_VULKAN_INSTANCE_HPP
#define LOVE_VULKAN_INSTANCE_HPP

#include <love/client/graphics/vulkan/command_pool.hpp>
#include <love/client/graphics/vulkan/frame_buffers.hpp>
#include <love/client/graphics/vulkan/graphics_device.hpp>
#include <love/client/graphics/vulkan/graphics_pipeline.hpp>
#include <love/client/graphics/vulkan/image_views.hpp>
#include <love/client/graphics/vulkan/render_pass.hpp>
#include <love/client/graphics/vulkan/swap_chain.hpp>
#include <love/client/graphics/window/window.hpp>

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
    class VulkanInstance {
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
                Window::Properties windowProperties{};
                GraphicsDevice::Properties graphicsDeviceProperties{};
                SwapChain::Properties swapChainProperties{};
                ImageViews::Properties imageViewProperties{};
                RenderPass::Properties renderPassProperties{};
                GraphicsPipeline::Properties graphicsPipelineProperties{};
                FrameBuffers::Properties frameBufferProperties{};
                CommandPool::Properties commandPoolProperties{};

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
            inline Window* window() const noexcept { return _window.get(); }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            ApplicationInfo _applicationInfo;
            Properties _properties;
            Library _vulkanLibrary;
            VkInstance _vulkanInstance = nullptr;
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

            std::unique_ptr<Window> _window = nullptr;
            std::unique_ptr<GraphicsDevice> _graphicsDevice = nullptr;
            std::unique_ptr<SwapChain> _swapChain = nullptr;
            std::unique_ptr<ImageViews> _imageViews = nullptr;
            std::unique_ptr<RenderPass> _renderPass = nullptr;
            std::unique_ptr<GraphicsPipeline> _graphicsPipeline = nullptr;
            std::unique_ptr<FrameBuffers> _frameBuffers = nullptr;
            std::unique_ptr<CommandPool> _commandPool = nullptr;

            void _log(const std::string& message) const noexcept;
            void _loadVulkanLibrary() noexcept;
            void _loadGlobalVulkanFunctions() const noexcept;
            void _loadInstanceVulkanFunctions() const noexcept;
            void _checkValidationLayerSupport(const std::vector<const char*>& layers) const noexcept;
            void _validateEnabledExtensions() noexcept;
            void _createVulkanInstance() noexcept;
            void _initializeGLFW() const noexcept;
            void _createVulkanObjects() noexcept;

            static void _defaultGLFWErrorCallback(int error, const char* description);
    };
}

#endif // LOVE_VULKAN_INSTANCE_HPP
