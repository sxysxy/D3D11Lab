#pragma once
#include "stdafx.h"
#include "renderer.h"
#include "global.h"
#include "../Utility/shapes.h"

class Bitmap : public Utility::ReferredObject {

	int _width, _height;

public:

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	ComPtr<ID3D11RenderTargetView> render_target_view;

	const int &width = _width, &height = _height;

	Bitmap(int w, int h);
	Bitmap(const std::wstring &filename);

	void FillRect(const Utility::Rect &rect);
};