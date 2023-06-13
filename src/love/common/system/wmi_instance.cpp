#ifdef _WIN32
#include "wmi_instance.hpp"

#include <sstream>

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

namespace love_engine {
    WMI_Instance::WMI_Instance() {
        _init_COM();
        _create_COM_Instance();
        _connect_WMI_Server();
    }

    WMI_Instance::~WMI_Instance() {
        if (_wbemService) _wbemService->Release();
        if (_wbemLocator) _wbemLocator->Release();
        if (initialized) CoUninitialize();
    }

    void WMI_Instance::_init_COM() noexcept {
        HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            // TODO "Failed to initialize COM library. Error code: 0x" << hex << hres
        }
        
        hres =  CoInitializeSecurity(
            NULL,
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities
            NULL                         // Reserved
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
        IID_IWbemLocator, (LPVOID *) &_wbemLocator);
        if (FAILED(hres)) {
            // TODO "Failed to create IWbemLocator object. Error code: 0x" << hex << hres
            CoUninitialize();
        }
    }

    void WMI_Instance::_connect_WMI_Server() noexcept {
        BSTR wmiNamespace = SysAllocString(L"ROOT\\CIMV2");
        HRESULT hres = _wbemLocator->ConnectServer(
            wmiNamespace, // Object path of WMI namespace
            NULL,         // User name. NULL = current user
            NULL,         // User password. NULL = current
            0,            // Locale. NULL indicates current
            NULL,         // Security flags.
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
            NULL,                        // Server principal name
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities
        );
        if (FAILED(hres)) {
            // TODO "Could not set COM proxy blanket. Error code: 0x" << hex << hres
            _wbemService->Release();
            _wbemLocator->Release();
            CoUninitialize();
        }
    }

    [[nodiscard]] std::vector<std::string> WMI_Instance::query_Devices(
        const std::string& key
    ) {
        std::stringstream queryBuffer("SELECT * FROM ");
        queryBuffer << key;
        BSTR wql = SysAllocString("WQL");
        BSTR query = SysAllocString(queryBuffer);
        IEnumWbemClassObject* wbemEnum = NULL;
        HRESULT hres = _wbemService->ExecQuery(
            wql,
            query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &wbemEnum
        );
        SysFreeString(wql);
        SysFreeString(query);
        if (FAILED(hres)) {
            // TODO "Query for " << key << " failed. Error code: 0x" << hex << hres
            _wbemService->Release();
            _wbemLocator->Release();
            CoUninitialize();
        }

        IWbemClassObject *wbemClassObj = NULL;
        ULONG uReturn = 0;
        using convert_utf8 = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_utf8, wchar_t> wstrConverter;
        std::vector<std::string> queryResults;
        while (wbemEnum) {
            hres = webmEnum->Next(WBEM_INFINITE, 1, &wbemClassObj, &uReturn);
            if(uReturn == 0) break;

            VARIANT variantProperty;
            VariantInit(&variantProperty);
            // Get the value of the Name property
            hres = wbemClassObj->Get(L"Name", 0, &variantProperty, 0, 0);
            queryResults.push_back(
                std::move(
                    wstrConverter.to_bytes(
                        std::wstring(
                            variantProperty.bstrVal,
                            SysStringLen(variantProperty.bstrVal)
                        )
                    ) 
                )
            );
            VariantClear(&variantProperty);

            wbemClassObj->Release();
        }
        
        wbemEnum->Release();
        if (queryResults.size() == 0) queryResults.push_back("Not found");
        return std::move(queryResults);
    }
}

#endif // _WIN32
