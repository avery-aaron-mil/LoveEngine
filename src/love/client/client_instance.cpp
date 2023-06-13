#include "client_instance.hpp"

#include <chrono>

namespace love_engine {

    void ClientInstance::run() noexcept {
        _running = true;
        
        auto previousTime = std::chrono::high_resolution_clock::now();
        std::float128_t lag = 0.0f;
        while (_running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::float128_t elapsedTime = std::chrono::duration<std::float128_t, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= _msPerTick) {
                if (_nextClientState) {
                    _clientState = _nextClientState;
                    _nextClientState = nullptr;
                    previousTime = std::chrono::high_resolution_clock::now();
                }

                _clientState->update();
                lag -= _msPerTick;
            }

            // Render object positions between ticks (input is percentage of next tick)
            // Example: Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            _clientState->render(lag / _msPerTick);
        }
    }

}
