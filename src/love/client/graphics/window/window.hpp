#ifndef LOVE_WINDOW_HPP
#define LOVE_WINDOW_HPP

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace love_engine {
    class Window {
        public:
            Window();
            ~Window();

            bool should_Close() const noexcept { return glfwWindowShouldClose(_window); }

            const GLFWwindow* get_GLFW_Window() const noexcept { return _window; }

        private:
            GLFWwindow* _window = nullptr;
    };
}

#endif // LOVE_WINDOW_HPP
