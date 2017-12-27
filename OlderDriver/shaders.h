#pragma once

#include "global.h"

class Renderer;

class Shader {
	
	virtual void CreateShaderFromBinary(char *, int) = 0;

public:

	void Create(const std::wstring &filename);

	ComPtr<ID3D11Buffer> cbuffer;

	void CreateConstantBuffer(int size);

	void FlushConstantBuffer(void *ptr);
};

class PixelShader : public Shader{
	virtual void CreateShaderFromBinary(char *bincode, int length);
public:
	ComPtr<ID3D11PixelShader> shader;
	ComPtr<ID3D11SamplerState> sampler;
	void CreateSampler();
};

class VertexShader : public Shader{

	virtual void CreateShaderFromBinary(char *bincode, int length);
	
public:
	char *pcode; int codelen;
	ComPtr<ID3D11VertexShader> shader;

	VertexShader() {
		pcode = 0;
	}
	~VertexShader() {
		if (pcode)delete pcode;
	}
	
};

class RenderPipeline {
public:
	VertexShader vshader;
	PixelShader pshader;

	ComPtr<ID3D11InputLayout> input_layout;

	void SetInputLayout(const std::string*, const DXGI_FORMAT *formats, int count);
};

