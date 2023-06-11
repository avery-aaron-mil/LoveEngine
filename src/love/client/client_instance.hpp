#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "client_states/client_state.hpp"

#include <assert>
#include <stdfloat>

namespace love_engine {

    class Client_Instance {
        public:
            Client_Instance(const Client_State *const client_State,
                const float64_t ms_Per_Tick
            ) : _client_State{client_State}, _ms_Per_Tick{ms_Per_Tick} {
                std::static_assert(client_State != nullptr);
                _client_State = client_State;
            }
            ~Client_Instance() {}
            
            void run() noexcept;
            
            void set_Client_State(const Client_State *const client_State) noexcept {
                _next_Client_State = client_State;
            }
            
        private:
            bool _running = false;
            float64_t _ms_Per_Tick = 20.f;
            Client_State *_client_State = nullptr;
            Client_State *_next_Client_State = nullptr;
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
