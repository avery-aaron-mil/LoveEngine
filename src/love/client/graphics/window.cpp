#include "window.hpp"

namespace love_engine {
    Window::Window() {
        //
    }
    Window::~Window() {
        if (_window) glfwDestroyWindow(_window);
    }
}
