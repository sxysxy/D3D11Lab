#include <stdafx.h>
#include "D3DDevice.h"
#include "D3DDeviceContext.h"
#include "DX.h"
#include "D3DTexture2D.h"

D3DTexture2D::D3DTexture2D(D3DDevice *device, int w, int h, bool stenciled = false){
    Initialize(device, w, h, stenciled);
}

D3DTexture2D::D3DTexture2D(D3DDevice *device, const cstring &filename, bool stenciled = false){
    Initialize(device, filename, stenciled);
}

D3DTexture2D::D3DTexture2D(ID3D11Texture2D * a_native_object, bool stenciled = false){
    Initialize(a_native_object, stenciled);
}

void D3DTexture2D::Initialize(D3DDevice *device, int w, int h, bool stenciled = false){
    _width = w, _height = h;

    D3D11_TEXTURE2D_DESC td;
    RtlZeroMemory(&td, sizeof td);
    td.Width = width;
    td.Height = height;
    td.MipLevels = td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    if (FAILED(device->native_device->CreateTexture2D(&td, nullptr, &native_texture2d))) {
        throw std::runtime_error("Failed to create Texture2D");
    }
    if(stenciled)CreateStencil();
}

void D3DTexture2D::Initialize(D3DDevice *device, const cstring &filename, bool stenciled = false){
    D3DX11_IMAGE_LOAD_INFO info;
    RtlZeroMemory(&info, sizeof info);
    info.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    info.Format = DXGI_FORMAT_R8G8_B8G8_UNORM;
    info.MipFilter = D3DX11_FILTER_LINEAR;
    info.MipLevels = D3DX11_DEFAULT;
    if (FAILED(D3DX11CreateTextureFromFileW(device->native_device.Get(), filename.c_str(), &info, nullptr,
        reinterpret_cast<ID3D11Resource **>(native_texture2d.GetAddressOf()), 0))) {
        throw std::runtime_error("Failed to Create Texture2D, Maybe the file does not exist.");
    }
    D3D11_TEXTURE2D_DESC desc;
    native_texture2d->GetDesc(&desc);
    _width = desc.Width;
    _height = desc.Height;

    CreateViews(device->native_device.Get());
    if (stenciled) CreateStencil();
}

void D3DTexture2D::Initialize(ID3D11Texture2D *a_native_object, bool stenciled = false){
    assert(a_native_object);

    ComPtr<ID3D11Device> device;
    native_texture2d = a_native_object;
    native_texture2d->GetDevice(&device);
    CreateViews(device.Get());
    if (stenciled)
        CreateStencil();
}

void D3DTexture2D::CreateViews(ID3D11Device *device) {
    D3D11_TEXTURE2D_DESC desc;
    native_texture2d->GetDesc(&desc);
    if ((desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) &&
        FAILED(device->CreateShaderResourceView(reinterpret_cast<ID3D11Resource *>(native_texture2d.Get()),
        0, &native_shader_resource_view))) {
        throw std::runtime_error("Failed to Create Texture's Shader Resource View");
    }

    if ((desc.BindFlags & D3D11_BIND_RENDER_TARGET) &&
        FAILED(device->CreateRenderTargetView(reinterpret_cast<ID3D11Resource *>(native_texture2d.Get()), 0,
        &native_render_target_view))) {
        throw std::runtime_error("Failed to Create Texture's Render Target View");
    }
}

void D3DTexture2D::CreateStencil() {
    assert(native_texture2d);

    D3D11_TEXTURE2D_DESC td;
    RtlZeroMemory(&td, sizeof td);
    td.Width = width;
    td.Height = height;
    td.MipLevels = td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ComPtr<ID3D11Device> device;
    native_texture2d->GetDevice(&device);
    HRESULT hr;
    if (FAILED(hr = device->CreateTexture2D(&td, nullptr, &native_stencil_buffer))) {
        throw std::runtime_error("Failed to create stencil buffer");
    }
    if (FAILED(hr = device->CreateDepthStencilView(native_stencil_buffer.Get(), 0,
        &native_depth_sencil_view))) {
        throw std::runtime_error("Failed to create depth stencil view");
    }
}

namespace Ext { namespace DX {
    namespace D3DTexture2D {  
        VALUE klass;

        void Init() {

        }
    }
} }

