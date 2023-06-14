#ifdef _WIN32
#ifndef LOVE_WMI_INSTANCE_HPP
#define LOVE_WMI_INSTANCE_HPP

#include <codecvt>
#include <locale>
#include <string>

#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif
#include <comdef.h>
#include <Wbemidl.h>

namespace love_engine {
    class WMI_Instance {
        friend class SystemInfo;
        protected:
            WMI_Instance();
            ~WMI_Instance();

            // WARNING: Must call ->Release() on the object when finished using.
            [[nodiscard]] IEnumWbemClassObject* query_System_Info(const wchar_t*const query) const noexcept;
            // WARNING: Must call ->Release() on the object before reassigning, or when finished using.
            [[nodiscard]] static IWbemClassObject* get_Next_Query_Object(IEnumWbemClassObject*const queryResults) noexcept;
            std::string get_Object_Value(IWbemClassObject*const wbemClassObj, const wchar_t*const obj) noexcept;

        private:
            void _init_COM() noexcept;
            void _create_COM_Instance() noexcept;
            void _connect_WMI_Server() noexcept;

            bool _initialized = false;
            IWbemLocator *_wbemLocator = nullptr;
            IWbemServices *_wbemService = nullptr;

            using convert_utf8 = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_utf8, wchar_t> _wstrConverter;
    };
}

#endif // LOVE_WMI_INSTANCE_HPP
#endif // _WIN32
