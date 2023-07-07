#ifdef _WIN32
#ifndef LOVE_WINDOWS_REGISTRY_HPP
#define LOVE_WINDOWS_REGISTRY_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace love_engine {
    class WindowsRegistry {
        public:
            static std::vector<std::wstring> getHKLMChildren(const std::wstring& registryKey) noexcept;
            static std::string getHKLMValueString(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
            static int64_t getHKLMValueI64(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
            static int32_t getHKLMValueI32(const std::wstring& registryKey, const std::wstring& registryValue) noexcept;
    };
}

#endif // LOVE_WINDOWS_REGISTRY_HPP
#endif // _WIN32
