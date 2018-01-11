#pragma once

#include <stdafx.h>

class D3DDevice : public Utility::ReferredObject {
    
public:
    ComPtr<ID3D11Device> native_device;
    ComPtr<IDXGIDevice> native_dxgi_device;
    ComPtr<IDXGIAdapter> native_dxgi_adapter;
    ComPtr<IDXGIFactory> native_dxgi_factory;

    D3DDevice(){}
    D3DDevice(D3D_DRIVER_TYPE t) {
        Initialize(t);
    }
    ~D3DDevice() {
        UnInitialize();
    }

    void Initialize(D3D_DRIVER_TYPE type);
    void UnInitialize() {
        native_device.ReleaseAndGetAddressOf();
        native_dxgi_device.ReleaseAndGetAddressOf();
        native_dxgi_factory.ReleaseAndGetAddressOf();
    }
    void QueryAdapterInfo(DXGI_ADAPTER_DESC *);

    virtual void Release() {};
};

namespace Ext {
    namespace DX {
        namespace D3DDevice {
            extern VALUE klass;
            

            void Init();
        }
    }
}