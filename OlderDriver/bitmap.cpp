#include "bitmap.h"
#include "renderer.h"
#include "global.h"
#include "stdafx.h"

Bitmap::Bitmap(int w, int h) {
	_width = w, _height = h;
}

Bitmap::Bitmap(const std::wstring &filename){
	D3DX11_IMAGE_LOAD_INFO info;
	RtlZeroMemory(&info, sizeof info);
	info.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.MipLevels = D3DX11_DEFAULT;
	info.MipFilter = D3DX11_FILTER_LINEAR;
	if (FAILED(D3DX11CreateTextureFromFileW(g_renderer->device.Get(), filename.c_str(), &info, nullptr,
		reinterpret_cast<ID3D11Resource **>(texture.GetAddressOf()), 0))) {
		throw std::runtime_error("创建贴图对象失败，不支持的文件格式或文件不存在");
	}
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);
	_width = desc.Width; _height = desc.Height;
	HRESULT hr1, hr2;

	hr1 = g_renderer->device->CreateShaderResourceView(reinterpret_cast<ID3D11Resource*>(texture.Get()), nullptr,
			&shader_resource_view);
	hr2 = g_renderer->device->CreateRenderTargetView(reinterpret_cast<ID3D11Resource*>(texture.Get()), nullptr,
		&render_target_view);

	if (FAILED(hr1) || FAILED(hr2)) {
		throw std::runtime_error("创建贴图对象失败");
	}
}



