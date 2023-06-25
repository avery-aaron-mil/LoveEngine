#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "client_state.hpp"

#include <love/common/error/crash.hpp>

#include <stdfloat>

namespace love_engine {

    class ClientInstance {
        public:
            ClientInstance(const ClientState *const clientState, const std::float32_t ms_Per_Tick)
            : _msPerTick(ms_Per_Tick), _clientState(clientState) {
                if (clientState == nullptr) Crash::crash("clientState must not be NULL.");
            }
            ~ClientInstance() = default;
            
            void run() noexcept;
            
            void set_ClientState(const ClientState *clientState) noexcept {
                _nextClientState = clientState;
            }
            
        private:
            bool _running = false;
            std::float32_t _msPerTick = 20.f;
            const ClientState *_clientState = nullptr;
            const ClientState *_nextClientState = nullptr;
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
