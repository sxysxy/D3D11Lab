#include "renderer.h"
#include "shaders.h"
#include "stdafx.h"
#include "bitmap.h"

Renderer::Renderer() {
    _phase = RENDERER_PREPARING;
	_frame_rate = -1;
    vsync = false;
	fpsctrl = new FPSCTRL;
	task_tim = 0;
	preempted = false;
}

Renderer::~Renderer() {
	delete fpsctrl;
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

    swap_chain->ResizeBuffers(1, w, h,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ComPtr<ID3D11Texture2D> back_buffer;
    swap_chain->GetBuffer(0,
        __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
    RCHECK(SUCCEEDED(device->CreateRenderTargetView(back_buffer.Get(), 0, &main_target)),
        L"创建主RenderTargetView失败");

    context->OMSetRenderTargets(1, main_target.GetAddressOf(), 0);
	
	SetViewport({ 0, 0, _width = w, _height = h });
}

void Renderer::Mainloop() {
	
    render_thread = std::thread([this]() {

        HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
        typedef NTSTATUS(__stdcall * pNtDelayExecution) (BOOL, PLARGE_INTEGER);
        pNtDelayExecution NtDelayExecution;
        RCHECK((NtDelayExecution = (pNtDelayExecution)GetProcAddress(hNtdll, "NtDelayExecution")), L"ntdll!NtDelayExecution加载失败");
		_phase ^= RENDERER_PREPARING;

		SetFrameRate(frame_rate);

        while ((phase & RENDERER_TERMINATED) == 0) {

			_phase &= ~(RENDERER_READY);
			_phase |= RENDERER_RENDERING;


			
			if (preempted) {
				SetDefaultTarget();
				Clear();
			}
            //perform tasks
			while (!tasks.empty()) {
				if (mtx_push_task.try_lock()) {
					RenderTask ct = tasks.front();
					tasks.pop();
					mtx_push_task.unlock();
					if (ct.call)ct.call(this);
				}
            }


			_phase ^= RENDERER_RENDERING;
			_phase |= RENDERER_READY;

			if (vsync) {
				RenderVsync();
				continue;
			}

			RenderImmediately();
			task_tim++;
			if (frame_rate <= 0)
				continue;
			else fpsctrl->Await();
        }
    });
}

void Renderer::SetDefaultTarget() {
    current_target = main_target.Get();
	context->OMSetRenderTargets(1, &current_target, 0);
	SetViewport({ 0, 0, width, height });
}

void Renderer::SetRenderTarget(Bitmap *bmp) {
	current_target = bmp->render_target_view.Get();
	context->OMSetRenderTargets(1, bmp->render_target_view.GetAddressOf(), 0);
	SetViewport({ 0, 0, bmp->width, bmp->height });
}

void Renderer::Clear() {
    static const FLOAT black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->ClearRenderTargetView(current_target, black);
}

void Renderer::RenderImmediately() {
    swap_chain->Present(0, 0);
}

void Renderer::RenderVsync() {
    swap_chain->Present(1, 0);
}

void Renderer::SetFrameRate(int f) {
	_frame_rate = f;
	fpsctrl->Restart(f);
}

void Renderer::BindPipeline(RenderPipeline *pipeline){
	BindVertexShader(&pipeline->vshader);
	BindPixelShader(&pipeline->pshader);
	g_renderer->context->IASetInputLayout(pipeline->input_layout.Get());

	render_pipeline = pipeline;
}

void Renderer::BindVertexShader(VertexShader *shader){
	context->VSSetShader(shader->shader.Get(), 0, 0);
	if (shader->cbuffer) {
		context->VSSetConstantBuffers(0, 1, shader->cbuffer.GetAddressOf());
	}
}

void Renderer::BindPixelShader(PixelShader *shader){
	context->PSSetShader(shader->shader.Get(), 0, 0);
	context->PSSetSamplers(0, 1, shader->sampler.GetAddressOf());
	if (shader->cbuffer) {
		context->PSSetConstantBuffers(0, 1, shader->cbuffer.GetAddressOf());
	}
}

void Renderer::PushTask(const std::function<void(Renderer *renderer)> &f) {
	while (true) {
		if (mtx_push_task.try_lock()) {
			tasks.push({ f, task_tim });
			mtx_push_task.unlock();
			break;
		}
	}
}

void Renderer::SetViewport(const RECT &rect) {
	viewport = rect;
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = rect.left;
	viewport.TopLeftY = rect.top;
	viewport.Width = (float)(rect.right - rect.left);
	viewport.Height = (float)(rect.bottom - rect.top);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	context->RSSetViewports(1, &viewport);
}

void Renderer::Preempt() {
	sync_mutex.lock();
	preempted = true;
	sync_mutex.unlock();
}

void Renderer::PreemptDisable() {
	sync_mutex.lock();
	preempted = false;
	sync_mutex.unlock();
}

namespace Renderer2D {
	RenderPipeline render_texture2d_pipeline;
	RenderPipeline render_shape2d_pipeline;

	void CreatePipelines() {
		render_shape2d_pipeline.vshader.Create(L"render_vs.bin");
		render_shape2d_pipeline.pshader.Create(L"render_ps.bin");
		render_shape2d_pipeline.SetInputLayout(
			std::initializer_list<std::string>({ "POSITION", "COLOR" }).begin(),
			std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT }).begin(),
			2);

		render_texture2d_pipeline.vshader.Create(L"texture_vs.bin");
		render_texture2d_pipeline.pshader.Create(L"texture_ps.bin");
		render_texture2d_pipeline.SetInputLayout(
			std::initializer_list<std::string>({ "POSITION", "TEXCOORD" }).begin(),
			std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT }).begin(),
			2);
		render_texture2d_pipeline.vshader.CreateConstantBuffer(64);
		
	}
	void DrawRect(Renderer *renderer, float position[4][2], float colors[4][4]) {

		struct TTTT{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		}vecs[4] = {
			{{ position[0][0], position[0][1], 0}, { colors[0][0], colors[0][1], colors[0][2], colors[0][3] }}, 
			{{ position[1][0], position[1][1], 0}, { colors[1][0], colors[1][1], colors[1][2], colors[1][3] }}, 
			{{ position[2][0], position[2][1], 0}, { colors[2][0], colors[2][1], colors[2][2], colors[2][3] }},
			{{ position[3][0], position[3][1], 0}, { colors[3][0], colors[3][1], colors[3][2], colors[3][3] }}
		};

		ComPtr<ID3D11Buffer> vbuffer;
		D3D11_BUFFER_DESC bd;
		RtlZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(vecs);
		D3D11_SUBRESOURCE_DATA data;
		RtlZeroMemory(&data, sizeof data);
		
		data.pSysMem = vecs;
		if (FAILED(renderer->device->CreateBuffer(&bd, &data, &vbuffer))) {
			throw std::runtime_error("创建顶点缓冲失败");
		}

		UINT stride = sizeof(TTTT);
		UINT offset = 0;
		renderer->context->IASetVertexBuffers(0, 1, vbuffer.GetAddressOf(), &stride, &offset);
		renderer->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		renderer->context->Draw(4, 0);
	}
}