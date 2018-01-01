#include "stdafx.h"
#include "client.h"
#include "renderer.h"
#include "shaders.h"
#include "bitmap.h"
#include "sprite.h"
using namespace Utility;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){

    Client client(L"对没错我就是叫紫妈qjwajdalkjdkdas", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(233);  //渲染线程帧率
	client.SetFrameRate(60);		   //逻辑线程帧率
	Renderer2D::CreatePipelines();     //创建2D渲染所需的常用渲染管线

	float positions[4][2] = {
		{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}
	};
	float colors[4][4] = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },   //red
		{ 0.0f, 1.0f, 0.0f, 1.0f },   //green
		{ 0.0f, 0.0f, 1.0f, 1.0f },   //blue
		{ 1.0f, 0.0f, 1.0f, 1.0f }    //purple
	};
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
			Renderer2D::DrawRect(renderer, positions, colors);			//在上面按照坐标比例缩放绘制一个矩形
																		//之间的颜色采用线性插值方法脑补
			renderer->BindPipeline(&Renderer2D::render_texture2d_pipeline);  //使用2D贴图渲染管线
			renderer->SetDefaultTarget();								//
			sprite->Render();											//绘制精灵
			
		});
		renderer->PreemptDisable();			//解除抢占，渲染线程得以继续工作（然后它会把这帧的任务完成）
	});

    return 0;
}