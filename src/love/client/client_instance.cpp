#include "client_instance.hpp"

#include <chrono>

namespace love_engine {

    void Client_Instance::run() noexcept {
        _running = true;
        
        auto previousTime = std::chrono::high_resolution_clock::now();
        float128_t lag = 0.0f;
        while (_running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float128_t elapsedTime = std::chrono::duration<float128_t, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= _ms_Per_Tick) {
                if (_next_Client_State) {
                    _client_State = _next_Client_State;
                    _next_Client_State = nullptr;
                    previousTime = std::chrono::high_resolution_clock::now();
                }

                _client_State->update();
                lag -= _ms_Per_Tick;
            }

            // Render object positions between ticks (input is percentage of next tick)
            // Example: Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            _client_State->render(lag / _ms_Per_Tick);
        }
    }

}
