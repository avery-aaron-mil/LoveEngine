#include "client_state_loading.hpp"

namespace example_game {

    void ClientState_Loading::update() noexcept {
        _shouldExit = true;
    }

    void ClientState_Loading::render(std::float128_t lag) noexcept {
        //
    }
}
