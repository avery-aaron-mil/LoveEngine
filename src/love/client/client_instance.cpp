#include "client_instance.hpp"

#include <chrono>

namespace love_engine {

    void ClientInstance::init() noexcept {
        //
    }

    void ClientInstance::run() noexcept {
        auto previousTime = std::chrono::high_resolution_clock::now();
        std::float128_t lag = 0.0f;
        while (!_clientState->should_Exit()) {
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
        }
    }

}
