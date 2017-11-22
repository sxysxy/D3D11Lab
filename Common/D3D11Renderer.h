#pragma once

#include "Common.h"

class D3D11Renderer {
public:
    ID3D11Device *d3dDevice;
    ID3D11DeviceContext *d3dimmContext;
    IDXGISwapChain *swapChain;
    ID3D11Texture2D *depthStencilBuffer;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11DepthStencilView *depthStencilView;
    D3D11_VIEWPORT screenViewport;
    D3D_DRIVER_TYPE d3dDriverType;

    void init(HWND hWnd);
    void __fastcall clear();

    void __fastcall present(int level);  //vsync level 0~4
};