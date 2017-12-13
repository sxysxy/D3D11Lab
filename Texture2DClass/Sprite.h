#pragma once

#include "stdafx.h"
#include "global.h"
#include "Texture2D.h"

class Sprite {

    ComPtr<ID3D11Buffer> vertex_buffer;
    struct Vertex {
        DirectX::XMFLOAT2 pos;
        DirectX::XMFLOAT2 tex;
    }vecs[6];

    struct ShaderParam {
        float width, height;
        float posx, posy;
    };


public:
    int x, y;
    float zoomx, zoomy;

    Texture2D *texture;

    Sprite() {}
    Sprite(Texture2D *tex);
    void Render();
};