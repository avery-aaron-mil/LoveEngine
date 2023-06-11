#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "client_state.hpp"

#include <stdfloat>

namespace love_engine {

    class Client_Instance {
        public:
            Client_Instance(const Client_State *const client_State, const std::float32_t ms_Per_Tick)
            : _ms_Per_Tick(ms_Per_Tick), _client_State(client_State) {
                // TODO crash_Assert(client_State != nullptr, "client_State must not be NULL.");
            }
            ~Client_Instance() {}
            
            void run() noexcept;
            
            void set_Client_State(const Client_State *client_State) noexcept {
                _next_Client_State = client_State;
            }
            
        private:
            bool _running = false;
            std::float32_t _ms_Per_Tick = 20.f;
            const Client_State *_client_State = nullptr;
            const Client_State *_next_Client_State = nullptr;
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
