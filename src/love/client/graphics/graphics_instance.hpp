#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <love/common/data/files/logger.hpp>
#include <love/common/system/library.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW

namespace love_engine {
    class GraphicsInstance {
        public:
            typedef struct ApplicationInfo_ {
                std::string name;
                uint8_t versionMajor = 0;
                uint8_t versionMinor = 0;
                uint8_t versionPatch = 0;
                std::shared_ptr<Logger> logger;
            } ApplicationInfo;

            GraphicsInstance(const ApplicationInfo& applicationInfo, const std::function<void(int, const char*)>& glfwErrorCallback);
            ~GraphicsInstance();

            VkInstance instance() const noexcept { return _vulkanInstance; }

        private:
            Library _vulkanLibrary;
            ApplicationInfo _applicationInfo;

            void _log(const std::string& message) const noexcept;
            void _loadVulkanLibrary() noexcept;
            void _loadGlobalVulkanFunctions() const noexcept;
            void _loadInstanceVulkanFunctions() const noexcept;
            void _createVulkanInstance() noexcept;
            void _initializeGLFW(const std::function<void(int, const char*)>& glfwErrorCallback) const noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
