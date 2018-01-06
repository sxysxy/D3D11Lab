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
    
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("render_vs.shader"), 0, 0, "main", "vs_4_0", 0, 0, 0, &VS, 0, 0)), 
        L"编译顶点着色器失败");
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("render_ps.shader"), 0, 0, "main", "ps_4_0", 0, 0, 0, &PS, 0, 0)),
        L"编译像素着色器失败");

    RCHECK(SUCCEEDED(d3dDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vs)), L"创建顶点着色器失败！")
    RCHECK(SUCCEEDED(d3dDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &ps)), L"创建像素着色器失败！");
    
    d3dimmContext->VSSetShader(vs, 0, 0);
    d3dimmContext->PSSetShader(ps, 0, 0);
    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
	if (FAILED(d3dDevice->CreateInputLayout(ied, 1,
		VS->GetBufferPointer(), VS->GetBufferSize(), &inputLayout))) {
		throw std::runtime_error("输入布局不正确");
	}
    d3dimmContext->IASetInputLayout(inputLayout);

	D3D11_BUFFER_DESC bd;
	RtlZeroMemory(&bd, sizeof bd);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = 16;
	if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &cbuffer_vs))) {
		throw std::runtime_error("创建常量缓冲区失败!");
	}
	if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &cbuffer_ps))) {
		throw std::runtime_error("创建常量缓冲区失败!");
	}

	d3dimmContext->PSSetConstantBuffers(0, 1, cbuffer_ps.GetAddressOf());

}

void D3D11Renderer::initGraphics() {

	DirectX::XMFLOAT4 vecs[4] = {
		{ 0.0f, 0.0f, 0.0f, 0.0f } ,
		{ 0.0f, 1.0f, 0.0f, 0.0f } ,
		{ 1.0f, 0.0f, 0.0f, 0.0f } ,
		{ 1.0f, 1.0f, 0.0f, 0.0f } ,
	};
	//{-0.5, -0.5}, {-0.5, 0.5}, {0.5, -0.5}{0.5, 0.5}

    D3D11_BUFFER_DESC bd;
    RtlZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vecs);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA data;
	RtlZeroMemory(&data, sizeof data);
	data.pSysMem = vecs;
    RCHECK(SUCCEEDED(d3dDevice->CreateBuffer(&bd, &data, &vb)), L"创建顶点缓冲区失败！");
}
