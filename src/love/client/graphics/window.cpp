#include "window.hpp"

#include "vulkan_functions.hpp"

#include <sstream>

#include <love/common/error/crash.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace love_engine {
    Window::Window(VkInstance vulkanInstance, const WindowProperties& properties, std::shared_ptr<Logger> logger)
     : _logger(logger), _properties(properties), _vulkanInstance(vulkanInstance) {
        _createWindow();
    }
    Window::~Window() {
        if (_window) glfwDestroyWindow(_window);
        if (_surface) vkDestroySurfaceKHR(_vulkanInstance, _surface, nullptr);
    }
    
    void Window::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }
    
    void Window::focusWindow() const noexcept {
        glfwShowWindow(_window);
        glfwFocusWindow(_window);
    }
    
    void Window::_createWindowSurface() noexcept {
        if (glfwCreateWindowSurface(_vulkanInstance, _window, nullptr, &_surface) != VK_SUCCESS) {
            Crash::crash("Failed to create window surface.");
        }
    }

    void Window::_getMonitor(const int monitorID, GLFWmonitor*& monitor, const GLFWvidmode*& videoMode) const noexcept {
        _log("Getting monitor...");
        if (monitorID > 0) {
            int count;
            GLFWmonitor** monitors = glfwGetMonitors(&count);
            if (monitors == nullptr) Crash::crash("No monitors found.");
            if (count <= monitorID) Crash::crash("Monitor ID exceeds monitor count.");
            monitor = monitors[monitorID];
        } else {
            monitor = glfwGetPrimaryMonitor();
        }
        if (monitor == nullptr) Crash::crash("No monitors found.");

        // Name
        const char* monitorName = glfwGetMonitorName(monitor);
        if (monitorName == nullptr) Crash::crash("GLFW failed to get monitor name.");

        // Video mode
        videoMode = glfwGetVideoMode(monitor);
        if (videoMode == nullptr) Crash::crash("GLFW failed to get video mode for monitor.");

        std::stringstream buffer;
        buffer << "Monitor details:\n\tName: " << monitorName
            << "\n\tWidth: " << videoMode->width
            << "\n\tHeight: " << videoMode->height
            << "\n\tRefresh rate: " << videoMode->refreshRate << "Hz"
            << "\n\tRGB bit depth: " << (videoMode->redBits + videoMode->blueBits + videoMode->greenBits)
                << " (" << videoMode->redBits << "+" << videoMode->greenBits << "+" << videoMode->blueBits << ")"
        ;
        _log(buffer.str());
    }

    void Window::setIcon(const std::string& iconPath) noexcept {
        _properties.iconPath.assign(iconPath);
        _setWindowIcon();
    }

    void Window::_setWindowIcon() noexcept {
        GLFWimage icons[1];
        try {
            FileIO::validatePath(_properties.iconPath);
        } catch (std::invalid_argument& e) {
            std::stringstream buffer;
            buffer << "Could not validate icon file path: " << e.what();
            Crash::crash(buffer.str());
        }
        icons[0].pixels = stbi_load(_properties.iconPath.c_str(), &icons[0].width, &icons[0].height, nullptr, 4);
        glfwSetWindowIcon(_window, 1, icons);
        stbi_image_free(icons[0].pixels);
    }

    void Window::_createWindow() noexcept {
        _log("Creating window...");

        // Get monitor
        GLFWmonitor* monitor;
        const GLFWvidmode* videoMode;
        _getMonitor(_properties.monitor, monitor, videoMode);

        // Input validation
        if ((_properties.width <= 0) || (_properties.height <= 0)) {
            _properties.width = videoMode->width / 1.5;
            _properties.height = videoMode->height / 1.5;
        }
        if ((_properties.x < 0) || (_properties.y < 0)) {
            _properties.x = (videoMode->width - _properties.width) / 2;
            _properties.y = (videoMode->height - _properties.height) / 2;
        }

        // Window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        // Create window
        _window = glfwCreateWindow(_properties.width, _properties.height, _properties.title.c_str(), nullptr, nullptr);
        if (_window == nullptr) Crash::crash("GLFW failed to create window.");
        glfwSetWindowPos(_window, _properties.x, _properties.y);
        glfwSetWindowUserPointer(_window, this);
        if (!_properties.iconPath.empty()) _setWindowIcon();
        _createWindowSurface();

        _log("Created window.");
    }
}
