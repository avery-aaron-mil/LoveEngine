#ifndef LOVE_CLIENT_INSTANCE_HPP
#define LOVE_CLIENT_INSTANCE_HPP

#include "client_state.hpp"
#include "graphics/graphics_instance.hpp"

#include <love/common/data/files/logger.hpp>
#include <love/common/error/crash.hpp>

#include <memory>
#include <stdfloat>

namespace love_engine {

    class ClientInstance {
        public:
            struct Properties {
                GraphicsInstance::Properties graphicsProperties;
                std::float32_t msPerTick = 20.f;
            };

            ClientInstance(ClientState *const clientState, const Properties& properties, std::shared_ptr<Logger> logger);
            ~ClientInstance() = default;

            void init() noexcept;
            void run() noexcept;

            inline void setClientState(ClientState *clientState) noexcept { _nextClientState = clientState; }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            ClientState* _clientState = nullptr;
            ClientState* _nextClientState = nullptr;

            GraphicsInstance _graphicsInstance {_properties.graphicsProperties, _logger};
            Window _window = _graphicsInstance.window();

            void _log(const std::string& message) const noexcept;
    };

}

#endif // LOVE_CLIENT_INSTANCE_HPP
