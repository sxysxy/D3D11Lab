#pragma once

#include "D3DDevice.h"
#include "D3DDeviceContext.h"
#include "DX.h"
#include <stdafx.h>
#include <extension.h>

class D3DTexture2D : Utility::ReferredObject{
public:
    ComPtr<ID3D11Texture2D> native_texture2d;
    ComPtr<ID3D11Texture2D> native_stencil_buffer;
    
    ComPtr<ID3D11ShaderResourceView> native_shader_resource_view;
    ComPtr<ID3D11RenderTargetView> native_render_target_view;
    ComPtr<ID3D11DepthStencilView> native_depth_sencil_view;

    ~D3DTexture2D() {UnInitialize(); }
    D3DTexture2D(){}                                                    
    D3DTexture2D(D3DDevice *device, int w, int h, bool stenciled);      //from params
    D3DTexture2D(const cstring &filename, bool stenciled);                     //from file
    void Initialize(D3DDevice *device, int w, int h, bool stenciled);
    void Initialize(const cstring &filename, bool stenciled);
    void UnInitialize() {
        native_texture2d.ReleaseAndGetAddressOf();
        native_stencil_buffer.ReleaseAndGetAddressOf();
        native_shader_resource_view.ReleaseAndGetAddressOf();
        native_render_target_view.ReleaseAndGetAddressOf();
        native_depth_sencil_view.ReleaseAndGetAddressOf();
    }
};

namespace Ext {
    namespace DX {
        namespace D3DTexture2D {
            extern VALUE klass;
            
            void Init();
        }
    }
}

