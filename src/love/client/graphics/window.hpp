#ifndef LOVE_WINDOW_HPP
#define LOVE_WINDOW_HPP

#include <functional>
#include <memory>
#include <string>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW
#include <GLFW/glfw3.h>

namespace love_engine {
    class Window {
        public:
            enum class WindowType {
                WINDOWED,
                WINDOWED_BORDERLESS,
                FULLSCREEN_BORDERLESS,
                FULLSCREEN_MONITOR,
            };
            struct WindowProperties {
                public:
                std::string title = "";
                std::string iconPath = "";
                int width = 0;
                int height = 0;
                int x = -1;
                int y = -1;
                int monitor = 0;
                WindowType windowType = WindowType::WINDOWED;

                protected:
                friend class Window;
                bool _resized = false;
                bool _focused = false;
                bool _fullscreen = false;
                int _windowedWidth = 0;
                int _windowedHeight = 0;
            };

            Window(VkInstance vulkanInstance, const WindowProperties& properties, std::shared_ptr<Logger> logger);
            ~Window();

            void setIcon(const std::string& iconPath) noexcept;
            void setWindowType(const WindowType& windowType) noexcept;
            void show() const noexcept;
            void focus() const noexcept;
            inline bool shouldClose() const noexcept { return glfwWindowShouldClose(_window); }

            inline const GLFWwindow* window() const noexcept { return _window; }
            inline VkSurfaceKHR surface() const noexcept { return _surface; }
            inline VkExtent2D extent() const noexcept { return _extent; }

            inline void setResizeCallback(const std::function<void(GLFWwindow*, int, int)>& callback) noexcept {
                _userResizeCallback = callback;
            }
            inline void setPositionCallback(const std::function<void(GLFWwindow*, int, int)>& callback) noexcept { 
                _userPositionCallback = callback;
            }
            inline void setFocusCallback(const std::function<void(GLFWwindow*, int)>& callback) noexcept {
                _userFocusCallback = callback;
            }

        private:
            std::shared_ptr<Logger> _logger;
            WindowProperties _properties;
            GLFWwindow* _window = nullptr;
            VkInstance _vulkanInstance;
            VkSurfaceKHR _surface;
            VkExtent2D _extent;

            void _log(const std::string& message) const noexcept;
            static void _updateWindowSize(Window* window) noexcept;
            void _createWindowSurface() noexcept;
            void _getMonitor(const int monitorID, GLFWmonitor*& monitor, const GLFWvidmode*& videoMode) const noexcept;
            GLFWmonitor* _getCurrentMonitor() const noexcept;
            void _setWindowIcon() noexcept;
            void _createWindow() noexcept;

            std::function<void(GLFWwindow*, int, int)> _userResizeCallback;
            std::function<void(GLFWwindow*, int, int)> _userPositionCallback;
            std::function<void(GLFWwindow*, int)> _userFocusCallback;

            static void _framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) noexcept;
            static void _positionCallback(GLFWwindow* glfwWindow, int x, int y) noexcept;
            static void _focusCallback(GLFWwindow* glfwWindow, int focused) noexcept;
    };
}

#endif // LOVE_WINDOW_HPP
