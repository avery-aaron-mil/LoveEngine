#include "client_instance.hpp"

#include <chrono>

namespace love_engine {

    ClientInstance::ClientInstance(ClientState *const clientState, const Settings& settings, std::shared_ptr<Logger> logger)
    : _logger(logger), _settings(settings), _clientState(clientState) {
        if (clientState == nullptr) Crash::crash("clientState must not be NULL.");
        init();
    }

    void ClientInstance::init() noexcept {
        _window.show();
    }
    
    void ClientInstance::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void ClientInstance::run() noexcept {
        _log("Running client...");

        auto previousTime = std::chrono::high_resolution_clock::now();
        std::float128_t lag = 0.0f;
        while (!_window.shouldClose() && !_clientState->shouldExit()) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::float128_t elapsedTime = std::chrono::duration<std::float128_t, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= _settings.msPerTick) {
                if (_nextClientState) {
                    _clientState = _nextClientState;
                    _nextClientState = nullptr;
                    previousTime = std::chrono::high_resolution_clock::now();
                }

                _clientState->update();
                lag -= _settings.msPerTick;
            }

            _clientState->render(lag / _settings.msPerTick);

            glfwPollEvents();
        }

        if (_window.shouldClose()) _log("Window closed by user.");
        else if (_clientState->shouldExit()) _log("Client state exited program.");
    }

    void ClientInstance::_defaultGLFWErrorCallback(int error, const char* description) {
        Crash::crash(std::string("Caught GLFW error with no callback: ") + description);
    }
}
