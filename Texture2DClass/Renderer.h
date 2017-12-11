#pragma once
#include "stdafx.h"

class Renderer {
public:

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
   D3D11_VIEWPORT screen_viewport;

   void Initialize();
   void Start();
   void Terminate();
};