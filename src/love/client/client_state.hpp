#ifndef LOVE_CLIENT_STATE_HPP
#define LOVE_CLIENT_STATE_HPP

#include <stdfloat>

namespace love_engine {

    class ClientState {
        public:
            virtual ~ClientState() {}
            
            virtual void update() const noexcept = 0;
            virtual void render(std::float128_t lag) const noexcept = 0;
    };
    
}

#endif // LOVE_CLIENT_STATE_HPP
