#include "Texture2D.h"
#include "global.h"

Texture2D::Texture2D(const wstring &filename) {
    D3DX11_IMAGE_LOAD_INFO info;
    RtlZeroMemory(&info, sizeof(info));
    info.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    info.MipLevels = D3DX11_DEFAULT;
    info.MipFilter = D3DX11_FILTER_LINEAR;
    RCHECK(SUCCEEDED(D3DX11CreateTextureFromFile(g_renderer.device.Get(), filename.c_str(), &info, 0,
        reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()), 0)), L"加载纹理失败!");
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    _width = desc.Width;
    _height = desc.Height;
    RCHECK(SUCCEEDED(g_renderer.device->CreateShaderResourceView(reinterpret_cast<ID3D11Resource*>(texture.Get()), 
        nullptr, &shader_resource_view)), L"创建ShaderResourceView失败");
    RCHECK(SUCCEEDED(g_renderer.device->CreateRenderTargetView(reinterpret_cast<ID3D11Resource*>(texture.Get()),
        nullptr, &render_target_view)), L"创建RenderTargetView失败");

}