#ifndef LOVE_CLIENT_STATE_HPP
#define LOVE_CLIENT_STATE_HPP

#include <stdfloat>

namespace love_engine {

    class ClientState {
        public:
            // Tick/update the client state.
            virtual void update() noexcept = 0;
            // Example:
            // Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            // @param lag Percentage of the way to next tick; "ms since last tick"/"ms per tick"
            virtual void render(std::float128_t lag) noexcept = 0;
            // Exits ClientInstance if true.
            bool shouldExit() const noexcept { return _shouldExit; }

        protected:
            bool _shouldExit = false;
    };
    
}

#endif // LOVE_CLIENT_STATE_HPP
