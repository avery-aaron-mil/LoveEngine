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

    void Window::_updateWindowSize(Window* window) noexcept {
        if (window == nullptr) Crash::crash("Window passed to _updateWindowSize() was null.");

        int width, height;
        glfwGetFramebufferSize(window->_window, &width, &height);
        window->_extent.width = static_cast<uint32_t>(width);
        window->_extent.height = static_cast<uint32_t>(height);
        window->_properties._resized = true;
    }

    void Window::setWindowType(const WindowType& windowType) noexcept {
        switch (windowType) {
            default:
            case WindowType::WINDOWED: {
                glfwSetWindowAttrib(_window, GLFW_DECORATED, GLFW_TRUE);
                if (_properties._fullscreen) {
                    _properties.width = _properties._windowedWidth;
                    _properties.height = _properties._windowedHeight;
                    glfwSetWindowMonitor(_window, nullptr, _properties.x, _properties.y, _properties.width, _properties.height, 0);
                    _updateWindowSize(this);
                    _properties._fullscreen = false;
                }
            } break;
            case WindowType::WINDOWED_BORDERLESS: {
                glfwSetWindowAttrib(_window, GLFW_DECORATED, GLFW_FALSE);
                if (_properties._fullscreen) {
                    _properties.width = _properties._windowedWidth;
                    _properties.height = _properties._windowedHeight;
                    glfwSetWindowMonitor(_window, nullptr, _properties.x, _properties.y, _properties.width, _properties.height, 0);
                    _updateWindowSize(this);
                    _properties._fullscreen = false;
                }
            } break;
            case WindowType::FULLSCREEN_BORDERLESS: {
                const GLFWvidmode* videoMode = glfwGetVideoMode(_getCurrentMonitor());
                glfwSetWindowAttrib(_window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowMonitor(_window, nullptr, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
                if (!_properties._fullscreen) {
                    _properties.width = videoMode->width;
                    _properties.height = videoMode->height;
                    _updateWindowSize(this);
                    _properties._fullscreen = true;
                }
            } break;
            case WindowType::FULLSCREEN_MONITOR: {
                GLFWmonitor* monitor = _getCurrentMonitor();
                const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
                glfwSetWindowAttrib(_window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowMonitor(_window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
                if (!_properties._fullscreen) {
                    _properties.width = videoMode->width;
                    _properties.height = videoMode->height;
                    _updateWindowSize(this);
                    _properties._fullscreen = true;
                }
            } break;
        }
    }

    void Window::show() const noexcept {
        glfwShowWindow(_window);
    }
    
    void Window::hide() const noexcept {
        glfwHideWindow(_window);
    }
    
    void Window::focus() const noexcept {
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
                << " (R" << videoMode->redBits << "+G" << videoMode->greenBits << "+B" << videoMode->blueBits << ")"
        ;
        _log(buffer.str());
    }

    GLFWmonitor* Window::_getCurrentMonitor() const noexcept {
        // Thanks to Shmo for this algorithm
        // https://stackoverflow.com/questions/21421074

        // Get window position
        int windowX, windowY, windowWidth, windowHeight;
        glfwGetWindowPos(_window, &windowX, &windowY);
        glfwGetWindowSize(_window, &windowWidth, &windowHeight);

        // Check all monitors for greatest overlap
        int monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        const GLFWvidmode* mode;
        GLFWmonitor* bestMonitor = nullptr;
        int monitorX, monitorY;
        int overlap, bestOverlap = 0;
        for (int i = 0; i < monitorCount; ++i) {
            mode = glfwGetVideoMode(monitors[i]);
            glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

            overlap = std::max(0,
                (std::min(windowX + windowWidth, monitorX + mode->width) - std::max(windowX, monitorX))
                * std::max(0, std::min(windowY + windowHeight, monitorY + mode->height) - std::max(windowY, monitorY))
            );

            if (overlap > bestOverlap) {
                bestOverlap = overlap;
                bestMonitor = monitors[i];
            }
        }

        return bestMonitor;
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
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GL_TRUE);
        glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

        // Create window
        switch (_properties.windowType) {
            default:
            case WindowType::WINDOWED: {
                _log("Opening window in windowed mode.");
                _window = glfwCreateWindow(_properties.width, _properties.height, _properties.title.c_str(), nullptr, nullptr);
            } break;
            case WindowType::WINDOWED_BORDERLESS: {
                _log("Opening window in borderless windowed mode.");
                glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
                _window = glfwCreateWindow(_properties.width, _properties.height, _properties.title.c_str(), nullptr, nullptr);
            } break;
            case WindowType::FULLSCREEN_BORDERLESS: {
                _log("Opening window in borderless fullscreen mode.");
                glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
                _window = glfwCreateWindow(videoMode->width, videoMode->height, _properties.title.c_str(), nullptr, nullptr);
                _properties._fullscreen = true;
            } break;
            case WindowType::FULLSCREEN_MONITOR: {
                _log("Opening window in fullscreen monitor mode.");
                _window = glfwCreateWindow(videoMode->width, videoMode->height, _properties.title.c_str(), monitor, nullptr);
                _properties._fullscreen = true;
            } break;
        }
        if (_window == nullptr) Crash::crash("GLFW failed to create window.");

        // Set windowed width
        _properties._windowedWidth = _properties.width;
        _properties._windowedHeight = _properties.height;
        if (_properties._fullscreen) {
            _properties.width = videoMode->width;
            _properties.height = videoMode->height;
        } else {
            glfwSetWindowPos(_window, _properties.x, _properties.y);
        }
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        _extent.width = static_cast<uint32_t>(width);
        _extent.height = static_cast<uint32_t>(height);

        // Design windoww
        glfwSetWindowUserPointer(_window, this);
        if (!_properties.iconPath.empty()) _setWindowIcon();
        _createWindowSurface();

        // Set window callbacks
        glfwSetFramebufferSizeCallback(_window, _framebufferResizeCallback);
        glfwSetWindowPosCallback(_window, _positionCallback);
        glfwSetWindowFocusCallback(_window, _focusCallback);

        _log("Created window.");
    }

    void Window::_framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) noexcept {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        _updateWindowSize(window);
        window->_properties.width = width;
        window->_properties.height = height;

        if (window->_userResizeCallback != nullptr) window->_userResizeCallback(glfwWindow, width, height);
    }

    void Window::_positionCallback(GLFWwindow* glfwWindow, int x, int y) noexcept {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->_properties.x = x;
        window->_properties.y = y;
        
        if (window->_userPositionCallback != nullptr) window->_userResizeCallback(glfwWindow, x, y);
    }

    void Window::_focusCallback(GLFWwindow* glfwWindow, int focused) noexcept {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->_properties._focused = focused;
        
        if (window->_userFocusCallback != nullptr) window->_userFocusCallback(glfwWindow, focused);
    }
}
