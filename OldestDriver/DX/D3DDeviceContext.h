#pragma once

#include <stdafx.h>
#include "D3DTexture2D.h"

class D3DDevice;

class D3DDeviceContext : public Utility::ReferredObject{
protected:
    ComPtr<ID3D11Device> native_device;
public:
    ComPtr<ID3D11DeviceContext> native_context;
    ComPtr<ID3D11CommandList> native_command_list;
    D3DDeviceContext() {};
    D3DDeviceContext(D3DDevice *device);
    void Initialize(D3DDevice *device);
    void UnInitialize() {
        native_context.ReleaseAndGetAddressOf();
    }
    void FinishiCommandList();

    virtual void Release() {};
};

class D3DDeviceImmdiateContext : public D3DDeviceContext {
public:
    D3DDeviceImmdiateContext(D3DDevice *device);
    void ExecuteCommandList(ComPtr<ID3D11CommandList> list);
};

namespace Ext {
    namespace DX {
        namespace D3DDeviceContext {
            extern VALUE klass;
            extern VALUE klass_immcontext;
            extern VALUE klass_command_list;

            void Init();
        }
    }
}