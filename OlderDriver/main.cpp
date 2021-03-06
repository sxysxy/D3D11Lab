#include "stdafx.h"
#include "client.h"
#include "renderer.h"
#include "shaders.h"
#include "bitmap.h"
#include "sprite.h"
#include "rubyext.h"
#include "libmsvcrt.h"
using namespace Utility;

int __cdecl cmain(int argc, char *argv_[]) {
	ruby_sysinit(&argc, (char***)&argv_);  //这个ruby_sysinit一定要有，哪怕不用命令行参数。
	{
		RUBY_INIT_STACK;
		ruby_init();
		MakeExt();
		char *argv[2];  //
		argv[0] = new char[10];
		argv[1] = new char[10];
		strcpy(argv[0], "null");
		strcpy(argv[1], "main.rb");
		int ret = ruby_run_node(ruby_options(2, argv));   //第二个参数是char**的
		delete argv[0];
		delete argv[1];
		return ret;
	}

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){
	MSVCRT::GetFunctions();

	/*
	if (GetFileAttributes(L"main.rb") == INVALID_FILE_ATTRIBUTES) {
		MessageBox(0, L"main.rb not found, stop.", L"Error", MB_OK);
		return 0;
	}

	return cmain(0, nullptr);
	*/
	
    Client client(L"对没错我就是叫紫妈qjwajdalkjdkdas", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(60);  //渲染线程帧率
	client.SetFrameRate(60);		   //逻辑线程帧率
	Renderer2D::CreatePipelines();     //创建2D渲染所需的常用渲染管线

	auto bitmap = ReferPtr<Bitmap>::New(L"../CommonFiles/250px-Yukari.jpg");   //美滋滋的自制智能指针
	auto sprite = ReferPtr<Sprite>::New(bitmap.Get());
	sprite->x = sprite->y = 175;
	client.Mainloop([&](Renderer *renderer) {
		renderer->Preempt();				//抢占当前绘图帧
		renderer->PushTask([&](Renderer *renderer) {
			renderer->SetDefaultTarget();		//设置当前渲染模板为默认目标（即显示屏上的窗口）
			renderer->Clear();					//清空渲染目标（屏幕）
			renderer->SetRenderTarget(bitmap.Get());	//设置当前渲染目标为bitmap
			renderer->BindPipeline(&Renderer2D::render_shape2d_pipeline);  //使用2D多边形绘制渲染管线
			Renderer2D::FillRect(Utility::Rect{ 100, 100, 100, 100 }, Utility::Color{ 0.0f, 1.0f, 1.0f, 1.0f });
																		
			renderer->BindPipeline(&Renderer2D::render_texture2d_pipeline);  //使用2D贴图渲染管线
			renderer->SetDefaultTarget();								//
			sprite->Render();											//绘制精灵
			
		});
		renderer->PreemptDisable();			//解除抢占，渲染线程得以继续工作（然后它会把这帧的任务完成）
	});
	
	
}
