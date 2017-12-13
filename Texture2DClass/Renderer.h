#pragma once
#include "stdafx.h"

class Renderer {
   int _width, _height;
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
   ComPtr<ID3D11DepthStencilView> depth_stencil_view;
   ComPtr<ID3D11Buffer> vertex_shader_cbuffer;
   bool vsync;

   void Initialize();
   void Start();
   void Terminate();

   void Resize(int w, int h);

   void Clear();
   void Present();
};