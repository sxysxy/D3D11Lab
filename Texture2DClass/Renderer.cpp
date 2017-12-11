#include "stdafx.h"
#include "global.h"
#include "Renderer.h"

void Renderer::Initialize() {
    quit = false;

    //Device, Context
    {
        D3D_FEATURE_LEVEL flevel;
        RCHECK(SUCCEEDED(D3D11CreateDevice(0,
            D3D_DRIVER_TYPE_HARDWARE,
            0, 0, 0, 0, D3D11_SDK_VERSION,
            &device, &flevel, &context)), L"创建D3D11设备失败")
        DXGI_SWAP_CHAIN_DESC sd;
        RtlZeroMemory(&sd, sizeof sd);
        RECT rect;
        GetClientRect(g_hMainwindow, &rect);
        sd.BufferCount = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.Windowed = true;
        sd.OutputWindow = g_hMainwindow;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.BufferDesc.Width = rect.right;
        sd.BufferDesc.Height = rect.bottom;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        ComPtr<IDXGIDevice> dxgi_device;
        RCHECK(SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice), 
            &dxgi_device)), L"创建DXGIDevice失败")
        ComPtr<IDXGIAdapter> dxgi_adpter;
        RCHECK(SUCCEEDED(dxgi_device->GetParent(__uuidof(IDXGIAdapter), 
            &dxgi_adpter)), L"创建DXGIAdpter失败")
        ComPtr<IDXGIFactory> dxgi_factory;
        RCHECK(SUCCEEDED(dxgi_adpter->GetParent(__uuidof(IDXGIFactory),
            &dxgi_factory)), L"创建DXGIFactory失败")
        RCHECK(SUCCEEDED(dxgi_factory->CreateSwapChain(device.Get(), &sd, 
            swap_chain.GetAddressOf())), L"创建图形接口交换链失败")
        
    }
    Start();
}

void Renderer::Start() {
    render_thread = std::thread([this](){
        while (!quit) {
            std::this_thread::yield();
        }
    });
}

void Renderer::Terminate() {
    quit = true;
    render_thread.join();
}