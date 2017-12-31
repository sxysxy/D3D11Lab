#include "renderer.h"
#include "global.h"
#include "shaders.h"

void Shader::Create(const std::wstring &filename) {
	FILE *file = _wfopen(filename.c_str(), L"rb");
	if (!file) 
		throw std::runtime_error("找不到文件");
	int length;
	fread(&length, 4, 1, file);
	char *buffer = new char[length];
	fread(buffer, length, 1, file);
	fclose(file);
	CreateShaderFromBinary(buffer, length);
}

void Shader::CreateConstantBuffer(int size) {
	if (size % 16) {
		throw std::invalid_argument("常量缓冲区大小必须为16的整数倍");
	}

	D3D11_BUFFER_DESC bd;
	RtlZeroMemory(&bd, sizeof bd);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = size;
	if (FAILED(g_renderer->device->CreateBuffer(&bd, nullptr, &cbuffer))) {
		throw std::runtime_error("创建常量缓冲区失败!");
	}
}

void Shader::FlushConstantBuffer(void *ptr){
	g_renderer->context->UpdateSubresource(cbuffer.Get(), 0, 0, ptr, 0, 0);
}

void PixelShader::CreateShaderFromBinary(char *code, int len) {
	g_renderer->device->CreatePixelShader(code, len, 0, &shader);
	delete code;
}

void PixelShader::CreateSampler() {
	D3D11_SAMPLER_DESC sd;
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //双线性
	sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 0;
	sd.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC> (0);
	RtlZeroMemory(sd.BorderColor, sizeof(sd.BorderColor));
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	ComPtr<ID3D11SamplerState> sampler;
	HRESULT hr = g_renderer->device->CreateSamplerState(&sd,
		&sampler);
	if (FAILED(hr))
		throw std::runtime_error("像素着色器创建采样器失败");
}


void VertexShader::CreateShaderFromBinary(char *code, int len) {
	g_renderer->device->CreateVertexShader(code, len, 0, &shader);
	pcode = code;
	codelen = len;
}

void RenderPipeline::SetInputLayout(const std::string *idents, const DXGI_FORMAT *formats, int count){
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied;

	for (int i = 0; i < count; i++) {
		ied.push_back(D3D11_INPUT_ELEMENT_DESC { idents[i].c_str(), 0, formats[i], 0,
							D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0});
	}

	g_renderer->device->CreateInputLayout(ied.data(), count, vshader.pcode, vshader.codelen, &input_layout);
}
