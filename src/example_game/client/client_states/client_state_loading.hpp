#ifndef EXAMPLE_CLIENT_STATE_LOADING_HPP
#define EXAMPLE_CLIENT_STATE_LOADING_HPP

#include <love/client/client_state.hpp>

using namespace love_engine;

namespace example_game {
    class ClientState_Loading : public ClientState {
        public:
            ClientState_Loading() {}
            ~ClientState_Loading() {}
            
            void update() const noexcept override;
            void render(std::float128_t lag) const noexcept override;
    };
}

#endif // EXAMPLE_CLIENT_STATE_LOADING_HPP
