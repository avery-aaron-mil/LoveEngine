#ifdef _WIN32
#include "wmi_instance.hpp"

namespace love_engine {
    WMI_Instance::WMI_Instance() {
        _init_COM();
        _create_COM_Instance();
        _connect_WMI_Server();
    }

    WMI_Instance::~WMI_Instance() {
        if (_wbemService) _wbemService->Release();
        if (_wbemLocator) _wbemLocator->Release();
        if (_initialized) CoUninitialize();
    }

    void WMI_Instance::_init_COM() noexcept {
        HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            // TODO "Failed to initialize COM library. Error code: 0x" << hex << hres
        }
        
        hres =  CoInitializeSecurity(
            nullptr,
            -1,                          // COM authentication
            nullptr,                     // Authentication services
            nullptr,                     // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
            nullptr,                     // Authentication info
            EOAC_NONE,                   // Additional capabilities
            nullptr                      // Reserved
        );
        if (FAILED(hres)) {
            // TODO "Failed to initialize COM security. Error code: 0x" << hex << hres
            CoUninitialize();
        }
        _initialized = true;
    }

    void WMI_Instance::_create_COM_Instance() noexcept {
        HRESULT hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID*) &_wbemLocator);
        if (FAILED(hres)) {
            // TODO "Failed to create IWbemLocator object. Error code: 0x" << hex << hres
            CoUninitialize();
        }
    }

    void WMI_Instance::_connect_WMI_Server() noexcept {
        BSTR wmiNamespace = SysAllocString(L"ROOT\\CIMV2");
        HRESULT hres = _wbemLocator->ConnectServer(
            wmiNamespace, // Object path of WMI namespace
            nullptr,      // User name. NULL = current user
            nullptr,      // User password. NULL = current
            0,            // Locale. NULL indicates current
            0,            // Security flags.
            0,            // Authority (for example, Kerberos)
            0,            // Context object 
            &_wbemService // pointer to IWbemServices proxy
        );
        SysFreeString(wmiNamespace);
        if (FAILED(hres)) {
            // TODO "Could not connect to WMI server. Error code: 0x" << hex << hres
            _wbemLocator->Release();
            CoUninitialize();
        }

        hres = CoSetProxyBlanket(
            _wbemService,                // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            nullptr,                     // Server principal name
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            nullptr,                     // client identity
            EOAC_NONE                    // proxy capabilities
        );
        if (FAILED(hres)) {
            // TODO "Could not set COM proxy blanket. Error code: 0x" << hex << hres
            _wbemService->Release();
            _wbemLocator->Release();
            CoUninitialize();
        }
    }

    [[nodiscard]] IEnumWbemClassObject* WMI_Instance::query_System_Info(const wchar_t*const query) const noexcept {
        BSTR wqlStr = SysAllocString(L"WQL");
        BSTR queryStr = SysAllocString(query);
        IEnumWbemClassObject* wbemEnum = nullptr;
        HRESULT hres = _wbemService->ExecQuery(
            wqlStr,
            queryStr,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            &wbemEnum
        );
        SysFreeString(wqlStr);
        SysFreeString(queryStr);
        if (FAILED(hres) || wbemEnum == nullptr) {
            // TODO "Query for " << key << " failed. Error code: 0x" << hex << hres
            _wbemService->Release();
            _wbemLocator->Release();
            CoUninitialize();
        }
        return wbemEnum;
    }

    [[nodiscard]] IWbemClassObject* WMI_Instance::get_Next_Query_Object(IEnumWbemClassObject*const queryResults) noexcept {
        IWbemClassObject* wbemClassObj = nullptr;
        ULONG uReturn = 0;
        queryResults->Next(WBEM_INFINITE, 1, &wbemClassObj, &uReturn);
        if(uReturn == 0 || wbemClassObj == nullptr) {
            // TODO "Could not get next query result on IEnumWbemClassObject. Error code: 0x" << std::hex << uReturn;
        }
        return wbemClassObj;
    }

    uint32_t WMI_Instance::get_Object_Value_UI32(
        IWbemClassObject*const wbemClassObj,
        const wchar_t*const obj
    ) noexcept {
        VARIANT variantProperty;
        VariantInit(&variantProperty);
        wbemClassObj->Get(obj, 0, &variantProperty, 0, 0);
        if (variantProperty.vt != VT_UI4) {
            VariantClear(&variantProperty);
            return -1; // TODO "Variant property was not UI4 when requesting a uint32_t." 
        }
        uint32_t value = variantProperty.uintVal;
        VariantClear(&variantProperty);
        return value;
    }
    uint64_t WMI_Instance::get_Object_Value_UI64(
        IWbemClassObject*const wbemClassObj,
        const wchar_t*const obj
    ) noexcept {
        VARIANT variantProperty;
        VariantInit(&variantProperty);
        wbemClassObj->Get(obj, 0, &variantProperty, 0, 0);
        if (variantProperty.vt != VT_UI8) {
            VariantClear(&variantProperty);
            return -1; // TODO "Variant property was not UI8 when requesting a uint64_t."
        }
        uint64_t value = variantProperty.ullVal;
        VariantClear(&variantProperty);
        return value;
    }
    std::string WMI_Instance::get_Object_Value_String(
        IWbemClassObject*const wbemClassObj,
        const wchar_t*const obj
    ) noexcept {
        VARIANT variantProperty;
        VariantInit(&variantProperty);
        wbemClassObj->Get(obj, 0, &variantProperty, 0, 0);
        if (variantProperty.vt != VT_BSTR) {
            VariantClear(&variantProperty);
            return "Variant property was not BSTR when requesting a string."; // TODO Crash 
        }
        std::string value = _wstrConverter.to_bytes(
            std::wstring(
                variantProperty.bstrVal,
                SysStringLen(variantProperty.bstrVal)
            )
        );
        VariantClear(&variantProperty);
        return value;
    }
}

#endif // _WIN32
