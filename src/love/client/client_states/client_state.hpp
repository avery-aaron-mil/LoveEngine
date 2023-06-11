#ifndef LOVE_CLIENT_STATE_HPP
#define LOVE_CLIENT_STATE_HPP

#include <stdfloat>

namespace love_engine {

    class Client_State {
        public:
            Client_State() {}
            ~Client_State() {}
            
            virtual void update() noexcept = 0;
            virtual void render(float128_t lag) noexcept = 0;
    };
    
}

#endif // LOVE_CLIENT_STATE_HPP
