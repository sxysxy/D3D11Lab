#include "stdafx.h"
#include "extension.h"
#include "HFWindow.h"
#include <regex>
#include <DX/D3DDevice.h>
#include <DX/SwapChain.h>
#include <DX/RenderPipeline.h>
#include <DX/D3DDeviceContext.h>
#include <DX/D3DTexture2D.h>
#include <DX/D3DBuffer.h>
#include <DX/Input.h>

using namespace Utility;

int __cdecl cmain(int argc, char *argv_[]) {
	ruby_sysinit(&argc, (char***)&argv_);  //这个ruby_sysinit一定要有，哪怕不用命令行参数。
	{
		RUBY_INIT_STACK;
		ruby_init();
		Ext::ApplyExtensions();

	    auto buffer = ReferPtr<HFBuffer<wchar_t>>::New(MAX_PATH+10);
        GetModuleFileNameW(GetModuleHandle(0), buffer->ptr, MAX_PATH);
        int len = lstrlenW(buffer->ptr);
        int p;
        for (p = len - 1; ~p; p--) {
            if(buffer->ptr[p] == L'\\')break;
        }
        buffer->ptr[p+1] = L'm';
        buffer->ptr[p+2] = L'a';
        buffer->ptr[p+3] = L'i';
        buffer->ptr[p+4] = L'n';
        buffer->ptr[p+5] = L'.';
        buffer->ptr[p+6] = L'r';
        buffer->ptr[p+7] = L'b';
        buffer->ptr[p+8] = 0;
        std::string filename;
        Ext::U16ToU8(buffer->ptr, filename, CP_UTF8);
		VALUE script = rb_str_new_cstr(filename.c_str());
		int state = 0;
		rb_load_protect(script, 0, &state);
		if (state) {
            VALUE errorinfo = rb_errinfo();
            rb_funcall(rb_mKernel, rb_intern("show_console"), 0);

            VALUE backtrance = rb_funcall(rb_make_backtrace(), rb_intern("to_s"), 0);
            rb_funcall(rb_mKernel, rb_intern("puts"), 1, backtrance);
            rb_funcall(rb_mKernel, rb_intern("puts"), 1, errorinfo);
            rb_eval_string("STDOUT.flush");
            system("pause");
		}
		return state;
	}

}

void JustTest1();
void JustTest2();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow) {
	MSVCRT::GetFunctions();
    CoInitialize(nullptr);
	Input::Initialize();

	if (GetFileAttributes(TEXT("main.rb")) == INVALID_FILE_ATTRIBUTES) {
	    MessageBox(0, TEXT("main.rb not found, stop."), TEXT("Error"), MB_OK);
	    return 0;
	}

	return cmain(0, nullptr);
    
	//JustTest1();
    //JustTest2();
    return 0;
}

void JustTest1() {
    auto window = ReferPtr<HFWindow>::New(L"Demo", 600, 600);
    window->SetFixed(true);
    window->Show();
    auto device = ReferPtr<D3DDevice>::New(D3D_DRIVER_TYPE_WARP);
    auto swap_chain = ReferPtr<SwapChain>::New(device.Get(), window.Get(), false);
    auto context = device->immcontext;
    struct Vertex {float pos[3], color[4]; };
    Vertex vecs[4] = {
        { { -0.5f, -0.5f, 0.0f } ,{ 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.0f } ,{ 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f } ,{ 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.0f},{ 0.0f, 0.0f, 1.0f, 1.0f } }
    }; //左下， 左上， 右下， 右上
    auto vbuffer = ReferPtr<D3DVertexBuffer>::New(device.Get(), sizeof vecs, (void*)vecs);
    auto pipeline = ReferPtr<RenderPipeline>::New();
    pipeline->vshader = VertexShader::LoadHLSLFile(device.Get(), L"render_vs.shader");
    pipeline->pshader = PixelShader::LoadHLSLFile(device.Get(), L"render_ps.shader");
    pipeline->SetInputLayout(device.Get(), 
        std::initializer_list<std::string>({ "POSITION" , "COLOR" }).begin(),
        std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT }).begin(),
    2);  
    context->BindPipeline(pipeline.Get());
    context->BindVertexBuffer(0, vbuffer.Get(), sizeof Vertex);
    context->SetViewport({ 0, 0, window->width, window->height });
    context->SetRenderTarget(&swap_chain->backbuffer);
    context->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0) {
            if (msg.message == WM_QUIT)break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            context->ClearRenderTarget(&swap_chain->backbuffer, 
                std::initializer_list<FLOAT>({ 0.0f, 0.0f, 0.0f, 1.0f }).begin());
            context->Draw(0, 4);
            swap_chain->Present(1);
        }
    }
}

void JustTest2() {
    auto window = ReferPtr<HFWindow>::New(L"恋恋!", 300, 300);
    window->SetFixed(true);
    window->Show();
    auto device = ReferPtr<D3DDevice>::New(D3D_DRIVER_TYPE_HARDWARE);
    auto swap_chain = ReferPtr<SwapChain>::New(device.Get(), window.Get());
    auto context = device->immcontext;
    auto texture = ReferPtr<D3DTexture2D>::New(device.Get(), L"../CommonFiles/300px-Komeiji Koishi.jpg", false);
    auto pipeline = ReferPtr<RenderPipeline>::New();
    pipeline->vshader = VertexShader::LoadHLSLFile(device.Get(), L"texture_vs.shader");
    pipeline->pshader = PixelShader::LoadHLSLFile(device.Get(), L"texture_ps.shader");
    pipeline->SetInputLayout(device.Get(),
        std::initializer_list<std::string>({ "POSITION", "TEXCOORD" }).begin(),
        std::initializer_list<DXGI_FORMAT>({ DXGI_FORMAT_R32G32_FLOAT,  DXGI_FORMAT_R32G32_FLOAT }).begin(),
        2);
    context->BindPipeline(pipeline.Get());
    auto sampler = ReferPtr<D3DSampler>::New();
    sampler->UseDefault();
    sampler->CreateState(device.Get());
    context->BindShaderSampler(0, sampler.Get(), SHADERS_APPLYTO_PSHADER);
    context->BindShaderResource(0, texture.Get(), SHADERS_APPLYTO_PSHADER);
    struct Vertex { float pos[2], tex[2]; };
    Vertex vecs[] = {
        {{-1.0f, -1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f},  {0.0f, 0.0f}},
        {{1.0f, -1.0f},  {1.0f, 1.0f}},
        {{1.0f, 1.0f},   {1.0f, 0.0f}}
    }; ////左下， 左上， 右下， 右上
    auto vbuffer = ReferPtr<D3DVertexBuffer>::New(device.Get(), sizeof vecs, reinterpret_cast<void *>(vecs));
    context->BindVertexBuffer(0, vbuffer.Get(), sizeof Vertex);
    context->SetRenderTarget(&swap_chain->backbuffer);
    context->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->SetViewport({0, 0, window->width, window->height});
    auto keyboard = ReferPtr<Input::Keyboard>::New(window->native_handle);
    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0) {
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            keyboard->ReadDeviceData();
            if(keyboard->IsKeyPressedNow(DIK_ESCAPE))break;
            if(keyboard->IsKeyPressedNow(DIK_A))window->MoveTo(200, 200);
            context->ClearRenderTarget(&swap_chain->backbuffer, 
                std::initializer_list<FLOAT>({0.0f, 0.0f, 0.0f, 0.0f}).begin());
            context->Draw(0, 4);
            swap_chain->Present(1);
        }
    }
}