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

        private:
            Library _vulkanLibrary;
            VkInstance _vulkanInstance;
            ApplicationInfo _applicationInfo;

            void _log(const std::string& message) const noexcept;
            void _load_Vulkan_Library() noexcept;
            void _load_Global_Vulkan_Functions() const noexcept;
            void _load_Instance_Vulkan_Functions() const noexcept;
            void _create_Vulkan_Instance() noexcept;
            void _initialize_GLFW(const std::function<void(int, const char*)>& glfwErrorCallback) const noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
