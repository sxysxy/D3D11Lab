#include "stdafx.h"
#include "global.h"
#include "Renderer.h"

void Renderer::Initialize() {
    quit = false;
    vsync = false;

    RECT rect;
    GetClientRect(g_hMainwindow, &rect);
    //Device, Context
    {
        D3D_FEATURE_LEVEL flevel;
        RCHECK(SUCCEEDED(D3D11CreateDevice(0,
            D3D_DRIVER_TYPE_HARDWARE,
            0, 0, 0, 0, D3D11_SDK_VERSION,
            &device, &flevel, &context)), L"创建D3D11设备失败")
        DXGI_SWAP_CHAIN_DESC sd;
        RtlZeroMemory(&sd, sizeof sd);
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

    //Back Buffer and Stencil Buffer
    {
        Resize(rect.right, rect.bottom);
    }

    //Shaders
    {
        ComPtr<ID3D10Blob> vs, ps;

        RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("vs.shader"), 0, 0, "main", "vs_4_0", 0, 0, 0, &vs, 0, 0)),
            L"编译顶点着色器失败");
        RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("ps.shader"), 0, 0, "main", "ps_4_0", 0, 0, 0, &ps, 0, 0)),
            L"编译像素着色器失败");

        D3D11_INPUT_ELEMENT_DESC ipdesc[2];
        //  POSITION
        ipdesc[0].SemanticName = "POSITION";
        ipdesc[0].SemanticIndex = 0U;
        ipdesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
        ipdesc[0].InputSlot = 0U;
        ipdesc[0].AlignedByteOffset = 0U;
        ipdesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        ipdesc[0].InstanceDataStepRate = 0U;
        //  TEXCOORD0
        ipdesc[1].SemanticName = "TEXCOORD";
        ipdesc[1].SemanticIndex = 0U;
        ipdesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        ipdesc[1].InputSlot = 0U;
        ipdesc[1].AlignedByteOffset = sizeof(DirectX::XMFLOAT2);
        ipdesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        ipdesc[1].InstanceDataStepRate = 0U;
        RCHECK(SUCCEEDED(device->CreateInputLayout(ipdesc, 2, 
            vs->GetBufferPointer(), vs->GetBufferSize(), &input_layout)), L"创建InputLayout失败");

        RCHECK(SUCCEEDED(device->CreateVertexShader(vs->GetBufferPointer(), 
                vs->GetBufferSize(), 0, &vertex_shader)), L"创建顶点着色器失败（也许您的硬件设备并不支持）");
        RCHECK(SUCCEEDED(device->CreatePixelShader(ps->GetBufferPointer(), 
            ps->GetBufferSize(), 0, &pixel_shader)), L"创建像素着色器失败（也许您的硬件并不支持）");
        context->VSSetShader(vertex_shader.Get(), 0, 0);
        context->PSSetShader(pixel_shader.Get(), 0, 0);
        context->IASetInputLayout(input_layout.Get());
        //VS Shader's Constant Buffer
        D3D11_BUFFER_DESC desc;
        RtlZeroMemory(&desc, sizeof desc);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = 64;
        RCHECK(SUCCEEDED(device->CreateBuffer(&desc, nullptr, &vertex_shader_cbuffer)), 
                L"创建顶点着色器常量缓冲区失败emm");
        context->VSSetConstantBuffers(0, 1, vertex_shader_cbuffer.GetAddressOf());
    }

    //PS Sampler
    {
        D3D11_SAMPLER_DESC desc;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;   //双线性
        desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 0;
        desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(0);
        RtlZeroMemory(desc.BorderColor, sizeof(desc.BorderColor));
        desc.MinLOD = 0.0f;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        ComPtr<ID3D11SamplerState> sampler;
        RCHECK(SUCCEEDED(device->CreateSamplerState(&desc, &sampler)), L"创建采样器失败");
        context->PSSetSamplers(0, 1, sampler.GetAddressOf());
    }


   // Start();
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
   // render_thread.join();
}

void Renderer::Resize(int w, int h) {
    RECT rect;
    GetClientRect(g_hMainwindow, &rect);

    swap_chain->ResizeBuffers(1, rect.right, rect.bottom, 
        DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ComPtr<ID3D11Texture2D> back_buffer;
    swap_chain->GetBuffer(0, 
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
    RCHECK(SUCCEEDED(device->CreateRenderTargetView(back_buffer.Get(), 0, &render_target_view)), 
        L"创建主RenderTargetView失败");
    
    //stencil buffer
    /*
    D3D11_TEXTURE2D_DESC desc;
    RtlZeroMemory(&desc, sizeof(desc));
    desc.Width = rect.right;
    desc.Height = rect.bottom;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> stencil_buffer;
    RCHECK(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, 
                &stencil_buffer)), L"创建深度缓存失败")
    device->CreateDepthStencilView(stencil_buffer.Get(), 
        0, depth_stencil_view.ReleaseAndGetAddressOf());
    context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
    */
    context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), 0);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)rect.right;
    viewport.Height = (float)rect.bottom;
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;
    context->RSSetViewports(1, &viewport);

    _width = w, _height = h;
}

void Renderer::Clear() {
    const FLOAT black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->ClearRenderTargetView(render_target_view.Get(), black);
    context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void Renderer::Present() {
    swap_chain->Present(vsync?1:0, 0);
}
