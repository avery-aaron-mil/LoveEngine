#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "client_state.hpp"

#include <love/common/error/crash.hpp>

#include <stdfloat>

namespace love_engine {

    class ClientInstance {
        public:
            typedef struct Settings_ {
                std::float32_t msPerTick = 20.f;
            } Settings;
            ClientInstance(ClientState *const clientState, const Settings settings)
            : _settings(settings), _clientState(clientState) {
                if (clientState == nullptr) Crash::crash("clientState must not be NULL.");
            }
            ~ClientInstance() = default;

            void run() noexcept;

            inline void set_ClientState(ClientState *clientState) noexcept { _nextClientState = clientState; }

        private:
            Settings _settings;
            ClientState *_clientState = nullptr;
            ClientState *_nextClientState = nullptr;
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
