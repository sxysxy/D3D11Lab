#pragma once

#pragma once

#include "stdafx.h"
#include "global.h"
#include "bitmap.h"

class Sprite : public Utility::ReferredObject{

	ComPtr<ID3D11Buffer> vertex_buffer;
	struct Vertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT2 tex;
	}vecs[6];

	struct ShaderParam {
		float width, height;
		float posx, posy;

		float sina, cosa;
	};


public:
	int x, y, ox, oy;
	float zoomx, zoomy;
	float angle;

	Utility::ReferPtr<Bitmap> bitmap;

	Sprite() {}
	Sprite(Bitmap *bmp);
	void Render();

    virtual void Release() {bitmap.Release();}
};
