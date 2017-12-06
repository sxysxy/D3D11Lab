#include "D3D11Renderer.h"

void D3D11Renderer::init(HWND hWnd) {
    RtlZeroMemory(this, sizeof(D3D11Renderer));

    UINT createDeviceFlag = 0;
    d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    //d3dDriverType = D3D_DRIVER_TYPE_WARP;

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

    RCHECK(SUCCEEDED(hr), L"创建D3D设备失败");
    RCHECK(SUCCEEDED(fLevel == D3D_FEATURE_LEVEL_11_0), L"D3D11_0特性不支持");

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

    ComPtr<IDXGIDevice> dxgiDevice;
    RCHECK(SUCCEEDED(d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)), L"创建DXGIDevice失败");

    ComPtr<IDXGIAdapter> dxgiAdapter;
    RCHECK(SUCCEEDED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)), L"创建DXGIAdapter失败");

    ComPtr<IDXGIFactory> dxgiFactory;
    RCHECK(SUCCEEDED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)), L"创建DXGIFactory失败");

    RCHECK(SUCCEEDED(dxgiFactory->CreateSwapChain(d3dDevice, &sd, &swapChain)), L"创建交换链失败");

    RCHECK(SUCCEEDED(swapChain->ResizeBuffers(1, r.right,
        r.bottom, DXGI_FORMAT_R8G8B8A8_UNORM, 0)), L"重设缓冲区大小失败");

    ComPtr<ID3D11Texture2D> backBuffer;

    RCHECK(SUCCEEDED(swapChain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()))), L"获取交换链缓冲区失败");
    RCHECK(SUCCEEDED(d3dDevice->CreateRenderTargetView(backBuffer.Get(), 0, &renderTargetView)), L"创建渲染目标视端失败");

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

    RCHECK(SUCCEEDED(d3dDevice->CreateTexture2D(&depthStencilDesc,
        0, &depthStencilBuffer)), L"创建深度缓存缓冲区失败");
    RCHECK(SUCCEEDED(d3dDevice->CreateDepthStencilView(depthStencilBuffer,
        0, &depthStencilView)), L"创建深度缓存视端失败");

    d3dimmContext->OMSetRenderTargets(1,
        &renderTargetView, depthStencilView);

    screenViewport.TopLeftX = 0;
    screenViewport.TopLeftY = 0;
    screenViewport.Width = (float)r.right;
    screenViewport.Height = (float)r.bottom;
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;
    d3dimmContext->RSSetViewports(1, &screenViewport);

    initPipeline();
    initGraphics();
}

void __fastcall D3D11Renderer::clear() {
    const FLOAT black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    d3dimmContext->ClearRenderTargetView(renderTargetView, black);
    d3dimmContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);   
}

void __fastcall D3D11Renderer::present(int level = 1) {
    swapChain->Present(level, 0);
}

void D3D11Renderer::initPipeline() {
    ComPtr<ID3D10Blob> VS, PS;
  
    /*
    std::shared_ptr <char> pvsc(new char[1000]), ppsc(new char[1000]);
    int vss, pss;
    FILE *fr = fopen("vs.vsb", "rb");
    vss = fread(pvsc.get(), 1, 1000, fr);
    fclose(fr);
    fr = fopen("ps.psb", "rb");
    pss = fread(ppsc.get(), 1, 1000, fr);
    fclose(fr);
    */

    
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("FirstShader.shader"), 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0)), 
        L"编译顶点着色器失败");
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("FirstShader.shader"), 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0)),
        L"编译像素着色器失败");
    

    //RCHECK(SUCCEEDED(d3dDevice->CreateVertexShader(pvsc.get(), vss, NULL, &vs)), L"创建顶点着色器失败！")
    //RCHECK(SUCCEEDED(d3dDevice->CreatePixelShader(ppsc.get(), pss, NULL, &ps)), L"创建像素着色器失败！");

    RCHECK(SUCCEEDED(d3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vs)), L"创建顶点着色器失败！")
    RCHECK(SUCCEEDED(d3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &ps)), L"创建像素着色器失败！");
    
    
    //store the shaders' binary code.
    /*
    FILE *ftmp = fopen("vs.vsb", "wb");
    fwrite(VS->GetBufferPointer(), VS->GetBufferSize(), 1, ftmp);
    fclose(ftmp);
    ftmp = fopen("ps.psb", "wb");
    fwrite(PS->GetBufferPointer(), PS->GetBufferSize(), 1, ftmp);
    fclose(ftmp);
    */
    

    d3dimmContext->VSSetShader(vs, 0, 0);
    d3dimmContext->PSSetShader(ps, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    d3dDevice->CreateInputLayout(ied, 2, 
        VS->GetBufferPointer(), VS->GetBufferSize(), &inputLayout);
 //   d3dDevice->CreateInputLayout(ied, 2, 
  //              pvsc.get(), vss, &inputLayout);
    d3dimmContext->IASetInputLayout(inputLayout);
}

void D3D11Renderer::initGraphics() {
    VERTEX vecs[100];
    RtlZeroMemory(vecs, sizeof(vecs));
    float x = -0.5, y = -0.5;
    for (int i = 0; i < 2; i++) {
        y = -0.5f;
        for (int j = 0; j < 2; j++) {
            vecs[j + i * 2] = { {x, y, 0}, {1.0f*(rand()&1), 1.0f*(rand() & 1), 1.0f*(rand() & 1), 1.0f} };
            y += 1.0f;
        }
        x += 1.0f;
    }

    D3D11_BUFFER_DESC bd;
    RtlZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX)*4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    RCHECK(SUCCEEDED(d3dDevice->CreateBuffer(&bd, NULL, &vb)), L"创建顶点缓冲区失败！");

    D3D11_MAPPED_SUBRESOURCE ms;
    d3dimmContext->Map(vb, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, vecs, sizeof(vecs));
    d3dimmContext->Unmap(vb, NULL);
}
