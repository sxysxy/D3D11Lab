#include "stdafx.h"
#include "extension.h"
#include "HFWindow.h"
#include <regex>
#include <DX/D3DDevice.h>
#include <DX/SwapChain.h>
#include <DX/RenderPipeline.h>
#include <DX/D3DDeviceContext.h>
#include <DX/D3DTexture2D.h>

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
            if(rb_eval_string("$DEBUG") != Qtrue);
                //return;

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

void JustTest();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow) {
	MSVCRT::GetFunctions();
	
	if (GetFileAttributes(TEXT("main.rb")) == INVALID_FILE_ATTRIBUTES) {
	    MessageBox(0, TEXT("main.rb not found, stop."), TEXT("Error"), MB_OK);
	    return 0;
	}

	//return cmain(0, nullptr);

	JustTest();
    return 0;
}

void JustTest() {
    auto window = ReferPtr<HFWindow>::New(L"2333", 600, 600);
    window->SetFixed(true);
    window->Show();
    auto device = ReferPtr<D3DDevice>::New(D3D_DRIVER_TYPE_HARDWARE);
    auto swap_chain = ReferPtr<SwapChain>::New(device.Get(), window.Get(), false);

    auto pipeline = ReferPtr<RenderPipeline>::New();
    pipeline->vshader = VertexShader::LoadHLSLFile(device.Get(), L"texture_vs.shader");
    pipeline->pshader = PixelShader::LoadHLSLFile(device.Get(), L"texture_ps.shader");
    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0) {
            if (msg.message == WM_QUIT)break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            swap_chain->Present(1);
        }
    }
}