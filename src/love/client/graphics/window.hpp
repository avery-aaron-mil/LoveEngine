#ifndef LOVE_WINDOW_HPP
#define LOVE_WINDOW_HPP

#include <memory>
#include <string>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW
#include <GLFW/glfw3.h>

namespace love_engine {
    class Window {
        public:
            struct WindowProperties {
                std::string title = "";
                std::string iconPath = "";
                int width = 0;
                int height = 0;
                int x = -1;
                int y = -1;
                int monitor = 0;
                bool fullscreen = false;
            };

            Window(VkInstance vulkanInstance, const WindowProperties& properties, std::shared_ptr<Logger> logger);
            ~Window();

            void setIcon(const std::string& iconPath) noexcept;
            void focusWindow() const noexcept;
            bool shouldClose() const noexcept { return glfwWindowShouldClose(_window); }

            inline const GLFWwindow* window() const noexcept { return _window; }
            inline VkSurfaceKHR surface() const noexcept { return _surface; }

        private:
            std::shared_ptr<Logger> _logger;
            WindowProperties _properties;
            GLFWwindow* _window = nullptr;
            VkInstance _vulkanInstance;
            VkSurfaceKHR _surface;

            void _log(const std::string& message) const noexcept;
            void _createWindowSurface() noexcept;
            void _getMonitor(const int monitorID, GLFWmonitor*& monitor, const GLFWvidmode*& videoMode) const noexcept;
            void _setWindowIcon() noexcept;
            void _createWindow() noexcept;
    };
}

#endif // LOVE_WINDOW_HPP
