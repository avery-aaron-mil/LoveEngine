#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "graphics/graphics_instance.hpp"
#include "graphics/graphics_device.hpp"
#include "graphics/window.hpp"
#include "client_state.hpp"

#include <love/common/data/files/logger.hpp>
#include <love/common/error/crash.hpp>

#include <functional>
#include <stdfloat>

namespace love_engine {

    class ClientInstance {
        public:
            struct Settings {
                GraphicsInstance::ApplicationInfo applicationInfo{};
                Window::WindowProperties windowProperties{};
                std::function<void(int, const char*)> glfwErrorCallback = _defaultGLFWErrorCallback;
                std::float32_t msPerTick = 20.f;
            };

            ClientInstance(ClientState *const clientState, const Settings& settings, std::shared_ptr<Logger> logger);
            ~ClientInstance() = default;

            void init() noexcept;
            void run() noexcept;

            inline void setClientState(ClientState *clientState) noexcept { _nextClientState = clientState; }

        private:
            std::shared_ptr<Logger> _logger;
            Settings _settings;
            ClientState* _clientState = nullptr;
            ClientState* _nextClientState = nullptr;
            
            GraphicsInstance _graphicsInstance{_settings.applicationInfo, _settings.glfwErrorCallback, _logger};
            Window _window{_graphicsInstance.instance(), _settings.windowProperties, _logger};
            GraphicsDevice _graphicsDevice{_graphicsInstance.instance(), _window.surface(), _logger};

            void _log(const std::string& message) const noexcept;
            static void _defaultGLFWErrorCallback(int error, const char* description);
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
