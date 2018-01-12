#ifdef _MSC_VER
#pragma warning(disable:4005 4390)  
#endif

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3dx11.h>
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11.lib")

const int width = 600;
const int height = 600;
const wchar_t *title = L"LoadTexture Test";

struct {
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
    D3D11_VIEWPORT screenViewport;
    D3D_DRIVER_TYPE driverType;
    ID3D11InputLayout *inputLayout;
    ID3D11VertexShader *vshader;
    ID3D11PixelShader *pshader;
    ID3D11Buffer *vbuffer;
}engine;

struct Vertex {
    DirectX::XMFLOAT2 pos, tex;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmd, int nCmdShow) {
    HWND hWnd;
    WNDCLASS wc;
    RtlZeroMemory(&wc, sizeof(wc));
    wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = LoadCursor(hInstance, IDI_WINLOGO);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
    };
    wc.lpszClassName = L"23333";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    RegisterClass(&wc);
    UINT wstyle = WS_OVERLAPPEDWINDOW &~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    RECT crect = { 0, 0, width, height };
    AdjustWindowRect(&crect, wstyle, false);
    hWnd = CreateWindow(L"23333", title, wstyle, 220, 233, crect.right - crect.left, crect.bottom - crect.top,
        0, 0, hInstance, 0);
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
    
    D3D_FEATURE_LEVEL flevel;
    RCHECK(SUCCEEDED(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
        0, 0, 0, 0, D3D11_SDK_VERSION, &engine.device, &flevel, &engine.context)), L"创建D3D设备失败");
    DXGI_SWAP_CHAIN_DESC spcd;
    RtlZeroMemory(&spcd, sizeof(spcd));
    GetClientRect(hWnd, &crect);
    spcd.BufferCount = 1;
    spcd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    spcd.Windowed = true;
    spcd.OutputWindow = hWnd;
    spcd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    spcd.BufferDesc.Width = crect.right;
    spcd.BufferDesc.Height = crect.bottom;
    spcd.BufferDesc.RefreshRate.Numerator = 60;
    spcd.BufferDesc.RefreshRate.Denominator = 1;
    spcd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    spcd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    spcd.SampleDesc.Count = 1;
    spcd.SampleDesc.Quality = 0;
    ComPtr<IDXGIDevice> dxgiDevice;
    RCHECK(SUCCEEDED(engine.device->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice)), L"");
    ComPtr<IDXGIAdapter> dxgiAdpter;
    RCHECK(SUCCEEDED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), &dxgiAdpter)), L"");
    ComPtr<IDXGIFactory> dxgiFactory;
    RCHECK(SUCCEEDED(dxgiAdpter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory)), L"");
    RCHECK(SUCCEEDED(dxgiFactory->CreateSwapChain(engine.device, &spcd, &engine.swapChain)), L"创建交换链失败");
    engine.swapChain->ResizeBuffers(1, crect.right, crect.bottom,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ComPtr<ID3D11Texture2D> backBuffer;
    engine.swapChain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    engine.device->CreateRenderTargetView(backBuffer.Get(), 0, &engine.renderTargetView);
    engine.context->OMSetRenderTargets(1, &engine.renderTargetView, 0);
    engine.screenViewport.TopLeftX = 0;
    engine.screenViewport.TopLeftY = 0;
    engine.screenViewport.Width = (float)crect.right;
    engine.screenViewport.Height = (float)crect.bottom;
    engine.screenViewport.MinDepth = 0.0f;
    engine.screenViewport.MaxDepth = 1.0f;
    engine.context->RSSetViewports(1, &engine.screenViewport);

    D3DX11_IMAGE_LOAD_INFO loadInfo;
    RtlZeroMemory(&loadInfo, sizeof(loadInfo));
    loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    loadInfo.MipLevels = D3DX11_DEFAULT;
    loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
    ComPtr<ID3D11Texture2D> texture;
    RCHECK(SUCCEEDED(D3DX11CreateTextureFromFile(engine.device, L"../CommonFiles/test.png", &loadInfo, 0,
        reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()), 0)), L"加载纹理失败!");
    D3D11_TEXTURE2D_DESC texdesc;
    texture->GetDesc(&texdesc);
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    RCHECK(SUCCEEDED(engine.device->CreateShaderResourceView(reinterpret_cast<ID3D11Resource*>(texture.Get()), nullptr, shaderResourceView.GetAddressOf())),
        L"创建纹理视口失败");
    engine.context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
    D3D11_SAMPLER_DESC desc;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 双线性
    desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.MipLODBias = .0f;
    desc.MaxAnisotropy = 0;
    desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(0);
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = .0f;
    desc.MinLOD = .0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    ComPtr<ID3D11SamplerState> sampler;
    RCHECK(SUCCEEDED(engine.device->CreateSamplerState(&desc, sampler.GetAddressOf())), L"创建采样器失败");
    engine.context->PSSetSamplers(0, 1, sampler.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC ipdesc[2];
    // 第 0 项是 POSITION
    ipdesc[0].SemanticName = "POSITION";
    ipdesc[0].SemanticIndex = 0U;
    ipdesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    ipdesc[0].InputSlot = 0U;
    ipdesc[0].AlignedByteOffset = 0U;
    ipdesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    ipdesc[0].InstanceDataStepRate = 0U;
    // 第 1 项是 TEXCOORD0
    ipdesc[1].SemanticName = "TEXCOORD";
    ipdesc[1].SemanticIndex = 0U;
    ipdesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    ipdesc[1].InputSlot = 0U;
    ipdesc[1].AlignedByteOffset = sizeof(DirectX::XMFLOAT2);
    ipdesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    ipdesc[1].InstanceDataStepRate = 0U;

    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D10Blob> pVShader, pPShader;
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("vs.shader"), 0, 0, "main", "vs_4_0", 0, 0, 0, &pVShader, 0, 0)),
        L"编译顶点着色器失败");
    RCHECK(SUCCEEDED(D3DX11CompileFromFile(_T("ps.shader"), 0, 0, "main", "ps_4_0", 0, 0, 0, &pPShader, 0, 0)),
        L"编译像素着色器失败");

    // 假设你编译完的 VSShader 在 VSShaderBlob 里面
    // VSShader 只是用来检查你的 InputLayout 是否合法
    RCHECK(SUCCEEDED(engine.device->CreateInputLayout(
        ipdesc,
        2U,
        pVShader->GetBufferPointer(),
        pVShader->GetBufferSize(),
        inputLayout.GetAddressOf()
    )), L"创建InputLayout失败");
    engine.context->IASetInputLayout(inputLayout.Get());
    RCHECK(SUCCEEDED(engine.device->CreateVertexShader(pVShader->GetBufferPointer(),
        pVShader->GetBufferSize(), 0, &engine.vshader)), L"创建顶点着色器失败");
    RCHECK(SUCCEEDED(engine.device->CreatePixelShader(pPShader->GetBufferPointer(), 
        pPShader->GetBufferSize(), 0, &engine.pshader)), L"创建像素着色器失败");
    engine.context->VSSetShader(engine.vshader, 0, 0);
    engine.context->PSSetShader(engine.pshader, 0, 0);

    //Vertex Buffer
    Vertex vertices[] = {
        {{1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f, 1.0f}, {0.0f, 1.0f}},
    };
    D3D11_BUFFER_DESC vbdesc;
    vbdesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
    vbdesc.Usage = D3D11_USAGE_DEFAULT;
    vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbdesc.CPUAccessFlags = 0U;
    vbdesc.MiscFlags = 0U;
    vbdesc.StructureByteStride = 0U;
    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = &vertices;

    RCHECK(SUCCEEDED(engine.device->CreateBuffer(
        &vbdesc,
        &data,
        &engine.vbuffer
    )), L"创建顶点缓冲区失败");


    HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
    typedef NTSTATUS(__stdcall * pNtDelayExecution) (BOOL, PLARGE_INTEGER);
    pNtDelayExecution NtDelayExecution;
    RCHECK((NtDelayExecution = (pNtDelayExecution)GetProcAddress(hNtdll, "NtDelayExecution")), L"ntdll!NtDelayExecution加载失败");
    long long tick = (long long)(-10000000.0 / 64.0 + 0.5);   //1 frame

    MSG msg;
    bool quit = false;
    while (!quit) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)quit = true;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            const FLOAT black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            engine.context->ClearRenderTargetView(engine.renderTargetView, black);
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            engine.context->IASetVertexBuffers(0, 1, &engine.vbuffer, &stride, &offset);
            engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            engine.context->Draw(6, 0);
            engine.swapChain->Present(0, 0);  //no vsync
            NtDelayExecution(false, (PLARGE_INTEGER)&tick);
        }
    }

    return 0;
}
