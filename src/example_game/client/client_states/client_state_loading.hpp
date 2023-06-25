#ifndef EXAMPLE_CLIENT_STATE_LOADING_HPP
#define EXAMPLE_CLIENT_STATE_LOADING_HPP

#include <love/client/client_state.hpp>

namespace example_game {
    class ClientState_Loading : public love_engine::ClientState {
        public:
            ClientState_Loading() = default;
            ~ClientState_Loading() = default;
            
            void update() const noexcept override;
            void render(std::float128_t lag) const noexcept override;
    };
}

#endif // EXAMPLE_CLIENT_STATE_LOADING_HPP
