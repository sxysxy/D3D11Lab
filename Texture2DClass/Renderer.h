#pragma once
#include "stdafx.h"
//#include "Texture2D.h"

class Texture2D;

struct Rect {
    int x, y, w, h;
};

struct Color {
    float r, g, b, a;
    Color() {
        r  = g = b = a = 0.0f;
    }
};

class Renderer {
   int _width, _height;
   Texture2D *render_target_texture;
   ID3D11RenderTargetView *current_target_view;

   ComPtr<ID3D11Buffer> draw_rect_vbuffer;

   struct Vertex {
       DirectX::XMFLOAT4 position, color;
   };

public:

   const int &width = _width, &height = _height;

   std::thread render_thread;
   BOOL quit;

   ComPtr<ID3D11Device> device;
   ComPtr<ID3D11DeviceContext> context;
   ComPtr<ID3D11VertexShader> vertex_shader;
   ComPtr<ID3D11PixelShader> pixel_shader;
   ComPtr<IDXGISwapChain> swap_chain;
   ComPtr<ID3D11Buffer> vertex_buffer;
   ComPtr<ID3D11InputLayout> input_layout;
   ComPtr<ID3D11RenderTargetView> render_target_view;
   ComPtr<ID3D11Buffer> vertex_shader_cbuffer;

   Color color;
   bool vsync;

   void Initialize();
   void Start();
   void Terminate();

   void Resize(int w, int h);

   void Clear();
   void Present();

   Texture2D *GetTargetTexture();
   
   void SetTargetTexture(Texture2D *tex);

   void DrawRect(const Rect &rect, const Color &color);
};