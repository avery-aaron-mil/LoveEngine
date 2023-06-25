#ifdef _WIN32
#include "windows_registry.hpp"

#include <codecvt>
#include <locale>
#include <Windows.h>

namespace love_engine {
    using convert_utf8 = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_utf8, wchar_t> _wstrConverter;

    std::string _get_System_Message(LSTATUS resultCode) noexcept {
        LPSTR systemMessageBuffer = nullptr;
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
            0,
            resultCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            systemMessageBuffer,
            0,
            nullptr
        );
        std::string systemMessage(systemMessageBuffer);
        LocalFree(systemMessageBuffer);
        return systemMessage;
    }

    std::vector<std::wstring> WindowsRegistry::get_HKLM_Children(const std::wstring& registryKey) noexcept {
        std::vector<std::wstring> children;
        HKEY parentKey;
        auto rc = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            0,
            KEY_ENUMERATE_SUB_KEYS,
            &parentKey
        );
        if (rc != ERROR_SUCCESS) {
            //throw std::runtime_error("Could not find registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc)); // TODO Crash
            return children;
        }

        size_t bufferSize = BUFSIZ;
        std::wstring childKeyBuf;
        DWORD cchName = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
        childKeyBuf.resize(bufferSize);
        for (DWORD i = 0; rc == ERROR_SUCCESS; ++i) {
            rc = RegEnumKeyExW(
                parentKey,
                i,
                reinterpret_cast<LPWSTR>(childKeyBuf.data()),
                &cchName,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            );
            if (rc == ERROR_MORE_DATA) {
                cchName /= sizeof(wchar_t);
                if (cchName > static_cast<DWORD>(bufferSize)) { // Check for overflow
                    bufferSize = static_cast<size_t>(cchName);
                } else { // Double bufferSize
                    bufferSize *= 2;
                    cchName = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
                }
                childKeyBuf.resize(bufferSize);
                rc = RegEnumKeyExW(
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
            if (rc == ERROR_SUCCESS) {
                children.push_back(std::move(childKeyBuf));
                continue;
            }
            else if (rc == ERROR_NO_MORE_ITEMS) break;
            else {
                RegCloseKey(parentKey);
                //throw std::runtime_error("Could not enumerate registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc)); // TODO Crash
                return children;
            }
        }

        RegCloseKey(parentKey);
        return children;
    }

    std::string WindowsRegistry::get_HKLM_Value_String(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        std::wstring valueBuf;
        size_t bufferSize = BUFSIZ;
        valueBuf.resize(bufferSize);
        DWORD cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
        auto rc = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(valueBuf.data()),
            &cbData
        );
        while (rc == ERROR_MORE_DATA) {
            cbData /= sizeof(wchar_t);
            if (cbData > static_cast<DWORD>(bufferSize)) { // Check for overflow
                bufferSize = static_cast<size_t>(cbData);
            } else { // Double bufferSize
                bufferSize *= 2;
                cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
            }
            valueBuf.resize(bufferSize);
            rc = RegGetValueW(
                HKEY_LOCAL_MACHINE,
                registryKey.c_str(),
                registryValue.c_str(),
                RRF_RT_REG_SZ,
                nullptr,
                reinterpret_cast<void*>(valueBuf.data()),
                &cbData
            );
        }
        if (rc == ERROR_SUCCESS) {
            cbData /= sizeof(wchar_t);
            valueBuf.resize(static_cast<size_t>(cbData - 1));
            return _wstrConverter.to_bytes(valueBuf);
        } else if (rc == ERROR_FILE_NOT_FOUND) {
            return "";
        } else {
            //throw std::runtime_error("Could not find registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc)); // TODO Crash
            return std::string("Could not find registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc));
        }
    }
    
    int64_t WindowsRegistry::get_HKLM_Value_I64(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        int64_t value = 0;
        DWORD cbData = static_cast<DWORD>(sizeof(int64_t));
        auto rc = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(&value),
            &cbData
        );
        if (rc == ERROR_SUCCESS) {
            return value;
        } else if (rc == ERROR_FILE_NOT_FOUND) {
            return -1;
        } else {
            //throw std::runtime_error("Could not find registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc)); // TODO Crash
            return -1;
        }
    }

    int32_t WindowsRegistry::get_HKLM_Value_I32(const std::wstring& registryKey, const std::wstring& registryValue) noexcept {
        int32_t value = 0;
        DWORD cbData = static_cast<DWORD>(sizeof(int32_t));
        auto rc = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            registryKey.c_str(),
            registryValue.c_str(),
            RRF_RT_ANY,
            nullptr,
            reinterpret_cast<void*>(&value),
            &cbData
        );
        if (rc == ERROR_SUCCESS) {
            return value;
        } else if (rc == ERROR_FILE_NOT_FOUND) {
            return -1;
        } else {
            //throw std::runtime_error("Could not find registry key. Windows system error code: " + std::to_string(rc) + ": " + _get_System_Message(rc)); // TODO Crash
            return -1;
        }
    }
}

#endif // _WIN32