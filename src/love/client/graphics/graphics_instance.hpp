#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP

#include <cstdint>
#include <functional>
#include <string>

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
            } ApplicationInfo;

            GraphicsInstance(const ApplicationInfo& applicationInfo, const std::function<void(int, const char*)>& glfwErrorCallback);
            ~GraphicsInstance();

        private:
            Library _vulkanLibrary;
            VkInstance _vulkanInstance;

            void _load_Global_Vulkan_Functions() noexcept;
            void _create_Vulkan_Instance(const ApplicationInfo& applicationInfo) noexcept;
            void _initialize_GLFW(
                const ApplicationInfo& applicationInfo,
                const std::function<void(int, const char*)>& glfwErrorCallback
            ) noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
