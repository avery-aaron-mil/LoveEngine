#ifndef EXAMPLE_CLIENT_STATE_LOADING_HPP
#define EXAMPLE_CLIENT_STATE_LOADING_HPP

#include <love/client/client_state.hpp>

using namespace love_engine;

namespace example_game {
    class Client_State_Loading : public Client_State {
        public:
            Client_State_Loading() {}
            ~Client_State_Loading() {}
            
            void update() const noexcept override;
            void render(std::float128_t lag) const noexcept override;
    };
}

#endif // EXAMPLE_CLIENT_STATE_LOADING_HPP
