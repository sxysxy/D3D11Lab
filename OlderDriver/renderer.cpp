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

void Renderer::MainloopProc(){

	SetFrameRate(frame_rate);
	while ((phase & RENDERER_TERMINATED) == 0) {

		_phase &= ~(RENDERER_READY);
		_phase |= RENDERER_RENDERING;


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
	render_pipeline->current = true;
}

void Renderer::BindVertexShader(VertexShader *shader){
	context->VSSetShader(shader->shader.Get(), 0, 0);
	if (shader->cbuffer) {
		context->VSSetConstantBuffers(0, 1, shader->cbuffer.GetAddressOf());
	}
}

void Renderer::BindPixelShader(PixelShader *shader){
	context->PSSetShader(shader->shader.Get(), 0, 0);
	if (shader->sampler)context->PSSetSamplers(0, 1, shader->sampler.GetAddressOf());
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
	viewport.TopLeftX = (float)rect.left;
	viewport.TopLeftY = (float)rect.top;
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
	ComPtr<ID3D11Buffer> render_shape2d_vbuffer;
	struct ColoredVertex {
		DirectX::XMFLOAT4 position, color;
	};
	void CreatePipelines() {
		render_shape2d_pipeline.vshader.Create(L"render_vs.bin");
		render_shape2d_pipeline.pshader.Create(L"render_ps.bin");
		render_shape2d_pipeline.SetInputLayout(
			std::initializer_list<std::string>({ "POSITION" , "COLOR"}).begin(),
			std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT}).begin(),
			2);
		
		ColoredVertex vecs[4] = {
			{{ -1.0f, -1.0f, 0.0f, 0.0f } , {0.0f, 0.0f, 1.0f, 1.0f}}, 
			{{ -1.0f, 1.0f, 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f, 1.0f }},
			{{ 1.0f, -1.0f, 0.0f, 0.0f } ,{0.0f, 0.0f, 1.0f, 1.0f}},
			{{ 1.0f, 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 1.0f, 1.0f}}
		}; //左下， 左上， 右下， 右上

		D3D11_BUFFER_DESC bd;
		RtlZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(vecs);
		D3D11_SUBRESOURCE_DATA data;
		RtlZeroMemory(&data, sizeof data);
		data.pSysMem = vecs;
		if (FAILED(g_renderer->device->CreateBuffer(&bd, &data, &render_shape2d_vbuffer))) {
			throw std::runtime_error("创建顶点缓冲失败");
		}

		//----------------------------------------------------------------------------------------
		render_texture2d_pipeline.vshader.Create(L"texture_vs.bin");
		render_texture2d_pipeline.pshader.Create(L"texture_ps.bin");
		render_texture2d_pipeline.pshader.CreateSampler();
		render_texture2d_pipeline.SetInputLayout(
			std::initializer_list<std::string>({ "POSITION", "TEXCOORD" }).begin(),
			std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT }).begin(),
			2);
		render_texture2d_pipeline.vshader.CreateConstantBuffer(32);

	}
	void FillRect(const Utility::Rect &rect, const Utility::Color &color){
		if (!Renderer2D::render_shape2d_pipeline.current) {
			throw std::runtime_error("不适宜的渲染管线");
		}

		//左下， 左上， 右下， 右上
		int midx = g_renderer->viewport.right / 2;
		int midy = g_renderer->viewport.bottom / 2;
		float x1 = 1.0f * (rect.x - midx) / midx;
		float x2 = 1.0f * (rect.x + rect.w - midx) / midx;
		float y2 = -1.0f * (rect.y - midy) / midy;
		float y1 = -1.0f * (rect.y + rect.h - midy) / midy;


		ColoredVertex vecs[4] = {
			{ { -1.0f, -1.0f, 0.0f, 0.0f } ,{ color.r, color.g, color.b, color.a } },
			{ { -1.0f, 1.0f, 0.0f, 0.0f } ,{ color.r, color.g, color.b, color.a } },
			{ { 1.0f, -1.0f, 0.0f, 0.0f } ,{ color.r, color.g, color.b, color.a } },
			{ { 1.0f, 1.0f, 0.0f, 0.0f },{ color.r, color.g, color.b, color.a } }
		}; //左下， 左上， 右下， 右上
		RECT ov = g_renderer->viewport;
		D3D11_VIEWPORT vp;
		vp.Height = rect.h;
		vp.Width = rect.w;
		vp.TopLeftX = rect.x;
		vp.TopLeftY = rect.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		g_renderer->context->RSSetViewports(1, &vp);

		/*
		ColoredVertex vecs[4] = {
			{ {x1, y1, 0.0f, 0.0f}, { color.r, color.g, color.b, color.a} },
			{ {x1, y2, 0.0f, 0.0f}, { color.r, color.g, color.b, color.a } },
			{ {x2, y1, 0.0f, 0.0f}, { color.r, color.g, color.b, color.a } },
			{ {x2, y2, 0.0f, 0.0f}, { color.r, color.g, color.b, color.a } }
		};
		*/

		g_renderer->context->UpdateSubresource(render_shape2d_vbuffer.Get(), 0, 0, vecs, 0, 0);
		UINT stride = sizeof(ColoredVertex);
		UINT offset = 0;
		g_renderer->context->IASetVertexBuffers(0, 1, render_shape2d_vbuffer.GetAddressOf(), &stride, &offset);
		g_renderer->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		g_renderer->context->Draw(4, 0);

		g_renderer->SetViewport(ov);
	}
}