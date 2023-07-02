#ifndef LOVE_GRAPHICS_INSTANCE_HPP
#define LOVE_GRAPHICS_INSTANCE_HPP

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <functional>
#include <string>

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
            VkInstance _vulkanInstance;
            void _initialize_Vulkan(const ApplicationInfo& applicationInfo, const char** extensions, uint32_t count) noexcept;
            void _initialize_GLFW(
                const ApplicationInfo& applicationInfo,
                const std::function<void(int, const char*)>& glfwErrorCallback
            ) noexcept;
    };
}

#endif // LOVE_GRAPHICS_INSTANCE_HPP
