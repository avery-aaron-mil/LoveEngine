#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "graphics/graphics_instance.hpp"
#include "client_state.hpp"

#include <love/common/error/crash.hpp>

#include <functional>
#include <stdfloat>

namespace love_engine {

    class ClientInstance {
        public:
            struct Settings {
                GraphicsInstance::ApplicationInfo applicationInfo{};
                std::function<void(int, const char*)> glfwErrorCallback = _defaultGLFWErrorCallback;
                std::float32_t msPerTick = 20.f;
            };
            ClientInstance(ClientState *const clientState, const Settings& settings)
            : _settings(settings), _clientState(clientState) {
                if (clientState == nullptr) Crash::crash("clientState must not be NULL.");
                init();
            }
            ~ClientInstance() = default;

            static void init() noexcept;
            void run() noexcept;

            inline void setClientState(ClientState *clientState) noexcept { _nextClientState = clientState; }

        private:
            Settings _settings;
            ClientState *_clientState = nullptr;
            ClientState *_nextClientState = nullptr;
            
            GraphicsInstance _graphicsInstance{_settings.applicationInfo, _settings.glfwErrorCallback};

            static void _defaultGLFWErrorCallback(int error, const char* description);
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
