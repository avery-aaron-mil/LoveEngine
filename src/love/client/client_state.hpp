#ifndef LOVE_CLIENT_STATE_HPP
#define LOVE_CLIENT_STATE_HPP

#include <stdfloat>

namespace love_engine {

    class Client_State {
        public:
            virtual ~Client_State() {}
            
            virtual void update() const noexcept = 0;
            virtual void render(std::float128_t lag) const noexcept = 0;
    };
    
}

#endif // LOVE_CLIENT_STATE_HPP
