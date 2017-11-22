#include "D3D11Renderer.h"

void D3D11Renderer::init(HWND hWnd) {
    RtlZeroMemory(this, sizeof(D3D11Renderer));

    UINT createDeviceFlag = 0;
    d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

    D3D_FEATURE_LEVEL fLevel;
    HRESULT hr = D3D11CreateDevice(
        0,
        d3dDriverType,
        0,
        createDeviceFlag,
        0, 0,
        D3D11_SDK_VERSION,
        &d3dDevice,
        &fLevel,
        &d3dimmContext
    );

    assert(SUCCEEDED(hr));
    assert(SUCCEEDED(fLevel == D3D_FEATURE_LEVEL_11_0));

    DXGI_SWAP_CHAIN_DESC sd;
    RtlZeroMemory(&sd, sizeof(sd));
    RECT r;
    GetClientRect(hWnd, &r);
    sd.BufferDesc.Width = r.right;
    sd.BufferDesc.Height = r.bottom;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.Windowed = true;
    sd.OutputWindow = hWnd;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    IDXGIDevice *dxgiDevice = 0;
    assert(SUCCEEDED(d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)));

    IDXGIAdapter *dxgiAdapter = 0;
    assert(SUCCEEDED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)));

    IDXGIFactory *dxgiFactory = 0;
    assert(SUCCEEDED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)));

    assert(SUCCEEDED(dxgiFactory->CreateSwapChain(d3dDevice, &sd, &swapChain)));

    dxgiDevice->Release();
    dxgiAdapter->Release();
    dxgiFactory->Release();

    assert(SUCCEEDED(swapChain->ResizeBuffers(1, r.right,
        r.bottom, DXGI_FORMAT_R8G8B8A8_UNORM, 0)));

    ID3D11Texture2D *backBuffer;
    assert(SUCCEEDED(swapChain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))));
    assert(SUCCEEDED(d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView)));
    backBuffer->Release();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    RtlZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.Width = r.right;
    depthStencilDesc.Height = r.bottom;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.MiscFlags = 0;

    assert(SUCCEEDED(d3dDevice->CreateTexture2D(&depthStencilDesc,
        0, &depthStencilBuffer)));
    assert(SUCCEEDED(d3dDevice->CreateDepthStencilView(depthStencilBuffer,
        0, &depthStencilView)));

    d3dimmContext->OMSetRenderTargets(1,
        &renderTargetView, depthStencilView);

    screenViewport.TopLeftX = 0;
    screenViewport.TopLeftY = 0;
    screenViewport.Width = (float)r.right;
    screenViewport.Height = (float)r.bottom;
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;
    d3dimmContext->RSSetViewports(1, &screenViewport);
}

void __fastcall D3D11Renderer::clear() {
    const FLOAT black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    d3dimmContext->ClearRenderTargetView(renderTargetView, black);
    d3dimmContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void __fastcall D3D11Renderer::present(int level = 1) {
    swapChain->Present(level, 0);
}