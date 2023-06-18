#ifdef _WIN32
#ifndef LOVE_WINDOWS_REGISTRY_HPP
#define LOVE_WINDOWS_REGISTRY_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace love_engine {
    class WindowsRegistry {
        public:
            static std::vector<std::wstring> get_HKLM_Children(const std::wstring& registryKey) noexcept;
            static std::string get_HKLM_Value_String(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
            static int64_t get_HKLM_Value_I64(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
            static int32_t get_HKLM_Value_I32(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
    };
}

#endif // LOVE_WINDOWS_REGISTRY_HPP
#endif // _WIN32
