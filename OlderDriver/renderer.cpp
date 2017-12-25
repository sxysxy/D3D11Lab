#include "renderer.h"

Renderer::Renderer() {
    _phase = RENDERER_PREPARING;
    frame_rate = -1;
    vsync = false;
    QueryPerformanceFrequency((PLARGE_INTEGER)&timer_freq);
}

void Renderer::Initialize(HWND hWnd) {
    window = hWnd;


    RECT rect;
    GetClientRect(window, &rect);
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
        sd.OutputWindow = window;
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

    {
        Resize(rect.right, rect.bottom);
    }
}

void Renderer::Resize(int w, int h) {
    RECT rect;
    GetClientRect(window, &rect);

    swap_chain->ResizeBuffers(1, rect.right, rect.bottom,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ComPtr<ID3D11Texture2D> back_buffer;
    swap_chain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
    RCHECK(SUCCEEDED(device->CreateRenderTargetView(back_buffer.Get(), 0, &main_target)),
        L"创建主RenderTargetView失败");

    context->OMSetRenderTargets(1, main_target.GetAddressOf(), 0);

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

void Renderer::Mainloop() {
	
    render_thread = std::thread([this]() {

        HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
        typedef NTSTATUS(__stdcall * pNtDelayExecution) (BOOL, PLARGE_INTEGER);
        pNtDelayExecution NtDelayExecution;
        RCHECK((NtDelayExecution = (pNtDelayExecution)GetProcAddress(hNtdll, "NtDelayExecution")), L"ntdll!NtDelayExecution加载失败");
		_phase ^= RENDERER_PREPARING;

		long long timer_freq;
		QueryPerformanceFrequency((PLARGE_INTEGER)&timer_freq);

        while ((phase & RENDERER_TERMINATED) == 0) {
    
			auto tick = 1.0 * timer_freq / frame_rate;
			auto judge = timer_freq;
            long long startt; QueryPerformanceCounter((PLARGE_INTEGER)&startt);

			_phase &= ~(RENDERER_READY);
			_phase |= RENDERER_RENDERING;

            SetDefaultTarget();
            Clear();

            //perform tasks
            while (!tasks.empty()) {
                RenderTask ct = tasks.front();
                tasks.pop();
                if(ct.call)ct.call(this);
            }

			_phase ^= RENDERER_RENDERING;
			_phase |= RENDERER_READY;

            if (vsync) {
                RenderVsync();
            }else {

				if (frame_rate > 0) {    //controls FPS;
					long long endt; 
					long long onems = -1000;
					while (true) {
						QueryPerformanceCounter((PLARGE_INTEGER)&endt);
						long long d = (long long)(tick - endt + startt);  //time left.
						if (d <= 0)break;

						//long long d = (long long)(endt - startt);  //time in total.
						//if (d >= tick)break;

						if (d >= 20000) {
							//NtDelayExecution(false, (PLARGE_INTEGER)&onems);
							Sleep(1);
						}
						else{
							std::this_thread::yield();
						}
					}
				}
				RenderImmediately();
            }
        }
    });
}

inline void Renderer::SetDefaultTarget() {
    current_target = main_target.Get();
}

inline void Renderer::Clear() {
    static const FLOAT black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->ClearRenderTargetView(current_target, black);
}

inline void Renderer::RenderImmediately() {
    swap_chain->Present(0, 0);
}

inline void Renderer::RenderVsync() {
    swap_chain->Present(1, 0);
}