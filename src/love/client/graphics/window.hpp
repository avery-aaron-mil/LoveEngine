#ifndef LOVE_WINDOW_HPP
#define LOVE_WINDOW_HPP

#include <GLFW/glfw3.h>

namespace love_engine {
    class Window {
        public:
            Window();
            ~Window();

            bool shouldClose() const noexcept { return glfwWindowShouldClose(_window); }

            const GLFWwindow* getGLFWWindow() const noexcept { return _window; }

        private:
            GLFWwindow* _window = nullptr;
    };
}

#endif // LOVE_WINDOW_HPP
