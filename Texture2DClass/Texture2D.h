#pragma once

#include "stdafx.h"
#include "global.h"

class Texture2D {
    int _height, _width;

public:
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    ComPtr<ID3D11RenderTargetView> render_target_view;  //��ʱ����ҪRender to texture

    const int &width = _width, &height = _height;

    Texture2D(const wstring &filename);

};