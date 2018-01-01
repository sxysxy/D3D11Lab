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

void Bitmap::FillRect(const Utility::Rect & rect) {
	struct Params {
		float x, y, width, height;
	}params { 2.0f * rect.x / g_renderer->viewport.right, 2.0f * rect.y / g_renderer->viewport.bottom,
				rect.w / g_renderer->viewport.right, rect.h / g_renderer->viewport.bottom};
	g_renderer->context->UpdateSubresource(Renderer2D::render_shape2d_pipeline.vshader.cbuffer.Get(), 0, 0, &params, 0, 0);

	/*
	ComPtr<ID3D11Buffer> vbuffer;
	D3D11_BUFFER_DESC bd;
	RtlZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(vecs);
	D3D11_SUBRESOURCE_DATA data;
	RtlZeroMemory(&data, sizeof data);

	data.pSysMem = vecs;
	if (FAILED(renderer->device->CreateBuffer(&bd, &data, &vbuffer))) {
		throw std::runtime_error("创建顶点缓冲失败");
	}

	UINT stride = sizeof(TTTT);
	UINT offset = 0;
	renderer->context->IASetVertexBuffers(0, 1, vbuffer.GetAddressOf(), &stride, &offset);
	renderer->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	renderer->context->Draw(4, 0);
	*/

}

