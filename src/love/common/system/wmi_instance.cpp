#include "wmi_instance.hpp"

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
        _wbemService->Release();
        _wbemLocator->Release();
        CoUninitialize();
    }

    void WMI_Instance::_init_COM() const noexcept {
        HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) {
            // TODO "Failed to initialize COM library. Error code = 0x" << hex << hres << "."
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
            // TODO "Failed to initialize COM security. Error code = 0x" << hex << hres << "."
            CoUninitialize();
        }
    }

    void WMI_Instance::_create_COM_Instance() noexcept {
        hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &_wbemLocator);
        if (FAILED(hres)) {
            // TODO "Failed to create IWbemLocator object. Error code = 0x" << hex << hres << "."
            CoUninitialize();
        }
    }

    void WMI_Instance::_connect_WMI_Server() noexcept {
        BSTR wmiNamespace = SysAllocString(L"ROOT\\CIMV2");
        hres = _wbemLocator->ConnectServer(
            wmiNamespace, // Object path of WMI namespace
            NULL,         // User name. NULL = current user
            NULL,         // User password. NULL = current
            0,            // Locale. NULL indicates current
            NULL,         // Security flags.
            0,            // Authority (for example, Kerberos)
            0,            // Context object 
            &_wbemService // pointer to IWbemServices proxy
        );
        if (FAILED(hres)) {
            // TODO "Could not connect to WMI server. Error code = 0x" << hex << hres << "."
            _wbemLocator->Release();
            CoUninitialize();
        }
        SysFreeString(wmiNamespace);

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
        if (FAILED(hres))
        {
            // TODO "Could not set COM proxy blanket. Error code = 0x" << hex << hres << "."
            _wbemService->Release();
            _wbemLocator->Release();
            CoUninitialize();
        }
    }
}
