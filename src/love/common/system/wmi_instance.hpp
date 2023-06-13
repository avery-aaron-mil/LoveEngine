#ifdef _WIN32
#ifndef LOVE_WMI_INSTANCE_HPP
#define LOVE_WMI_INSTANCE_HPP

#include <string>

namespace love_engine {
    class WMI_Instance {
        public:
            WMI_Instance();
            ~WMI_Instance();

            [[nodiscard]] std::vector<std::string> query_Devices();

        private:
            void _init_COM() const noexcept;
            void _create_COM_Instance() noexcept;
            void _connect_WMI_Server() noexcept;

            IWbemLocator *_wbemLocator = nullptr;
            IWbemServices *_wbemService = nullptr;
    };
}

#endif // LOVE_WMI_INSTANCE_HPP
#endif // _WIN32
