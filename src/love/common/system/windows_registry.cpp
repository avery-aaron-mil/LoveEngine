#ifdef _WIN32
#include <love/common/system/windows_registry.hpp>

#include <sstream>

#include <codecvt>
#include <locale>
#include <windows.h>

#include <love/common/error/crash.hpp>
#include <love/common/error/win32_error.hpp>

namespace love_engine {
    using convert_utf8 = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_utf8, wchar_t> _wstrConverter;

    std::vector<std::wstring> WindowsRegistry::getHKLMChildren(const std::wstring& registryKey) noexcept {
        std::vector<std::wstring> children;
        HKEY parentKey;
        auto result = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            0,
            KEY_ENUMERATE_SUB_KEYS,
            &parentKey
        );
        if (result != ERROR_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Could not find registry key. Windows system error (" <<
                std::to_string(result) << "): " << Win32Error::getSystemMessage(result);
            Crash::crash(crashBuffer.str());
        }

        size_t bufferSize = BUFSIZ;
        std::wstring childKeyBuf;
        DWORD cchName = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
        childKeyBuf.resize(bufferSize);
        for (DWORD i = 0; result == ERROR_SUCCESS; ++i) {
            result = RegEnumKeyExW(
                parentKey,
                i,
                reinterpret_cast<LPWSTR>(childKeyBuf.data()),
                &cchName,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            );
            if (result == ERROR_MORE_DATA) {
                cchName /= sizeof(wchar_t);
                if (cchName > static_cast<DWORD>(bufferSize)) { // Check for overflow
                    bufferSize = static_cast<size_t>(cchName);
                } else { // Double bufferSize
                    bufferSize *= 2;
                    cchName = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
                }
                childKeyBuf.resize(bufferSize);
                result = RegEnumKeyExW(
                    parentKey,
                    i,
                    reinterpret_cast<LPWSTR>(childKeyBuf.data()),
                    &cchName,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr
                );
            }
            if (result == ERROR_SUCCESS) {
                children.push_back(std::move(childKeyBuf));
                continue;
            }
            else if (result == ERROR_NO_MORE_ITEMS) break;
            else {
                RegCloseKey(parentKey);
                std::stringstream crashBuffer;
                crashBuffer << "Could not enumerate registry key. Windows system error (" <<
                    std::to_string(result) << "): " << Win32Error::getSystemMessage(result);
                Crash::crash(crashBuffer.str());
            }
        }

        RegCloseKey(parentKey);
        return children;
    }

    std::string WindowsRegistry::getHKLMValueString(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        std::wstring valueBuf;
        size_t bufferSize = BUFSIZ;
        valueBuf.resize(bufferSize);
        DWORD cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
        auto result = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(valueBuf.data()),
            &cbData
        );
        while (result == ERROR_MORE_DATA) {
            cbData /= sizeof(wchar_t);
            if (cbData > static_cast<DWORD>(bufferSize)) { // Check for overflow
                bufferSize = static_cast<size_t>(cbData);
            } else { // Double bufferSize
                bufferSize *= 2;
                cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
            }
            valueBuf.resize(bufferSize);
            result = RegGetValueW(
                HKEY_LOCAL_MACHINE,
                registryKey.c_str(),
                registryValue.c_str(),
                RRF_RT_REG_SZ,
                nullptr,
                reinterpret_cast<void*>(valueBuf.data()),
                &cbData
            );
        }
        if (result == ERROR_SUCCESS) {
            cbData /= sizeof(wchar_t);
            valueBuf.resize(static_cast<size_t>(cbData - 1));
            return _wstrConverter.to_bytes(valueBuf);
        } else if (result == ERROR_FILE_NOT_FOUND) {
            return "Not found";
        } else {
            std::stringstream crashBuffer;
            crashBuffer << "Could not find registry key. Windows system error (" <<
                std::to_string(result) << "): " << Win32Error::getSystemMessage(result);
            Crash::crash(crashBuffer.str());
        }
    }
    
    int64_t WindowsRegistry::getHKLMValueI64(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        int64_t value = 0;
        DWORD cbData = static_cast<DWORD>(sizeof(int64_t));
        auto result = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(&value),
            &cbData
        );
        if (result == ERROR_SUCCESS) {
            return value;
        } else if (result == ERROR_FILE_NOT_FOUND) {
            return -1;
        } else {
            std::stringstream crashBuffer;
            crashBuffer << "Could not find registry key. Windows system error (" <<
                std::to_string(result) << "): " << Win32Error::getSystemMessage(result);
            Crash::crash(crashBuffer.str());
        }
    }

    int32_t WindowsRegistry::getHKLMValueI32(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        int32_t value = 0;
        DWORD cbData = static_cast<DWORD>(sizeof(int32_t));
        auto result = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(&value),
            &cbData
        );
        if (result == ERROR_SUCCESS) {
            return value;
        } else if (result == ERROR_FILE_NOT_FOUND) {
            return -1;
        } else {
            std::stringstream crashBuffer;
            crashBuffer << "Could not find registry key. Windows system error (" <<
                std::to_string(result) << "): " << Win32Error::getSystemMessage(result);
            Crash::crash(crashBuffer.str());
        }
    }
}

#endif // _WIN32
