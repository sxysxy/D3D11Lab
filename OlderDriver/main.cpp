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
	ruby_sysinit(&argc, (char***)&argv_);  //���ruby_sysinitһ��Ҫ�У����²��������в�����
	{
		RUBY_INIT_STACK;
		ruby_init();
		MakeExt();
		char *argv[2];  //
		argv[0] = new char[10];
		argv[1] = new char[10];
		strcpy(argv[0], "null");
		strcpy(argv[1], "main.rb");
		int ret = ruby_run_node(ruby_options(2, argv));   //�ڶ���������char**��
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

	
    Client client(L"��û���Ҿ��ǽ�����qjwajdalkjdkdas", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(60);  //��Ⱦ�߳�֡��
	client.SetFrameRate(60);		   //�߼��߳�֡��
	Renderer2D::CreatePipelines();     //����2D��Ⱦ����ĳ�����Ⱦ����

	auto bitmap = ReferPtr<Bitmap>::New(L"../CommonFiles/250px-Yukari.jpg");   //�����̵���������ָ��
	auto sprite = ReferPtr<Sprite>::New(bitmap.Get());
	sprite->x = sprite->y = 175;
	client.Mainloop([&](Renderer *renderer) {
		renderer->Preempt();				//��ռ��ǰ��ͼ֡
		renderer->PushTask([&](Renderer *renderer) {
			renderer->SetDefaultTarget();		//���õ�ǰ��Ⱦģ��ΪĬ��Ŀ�꣨����ʾ���ϵĴ��ڣ�
			renderer->Clear();					//�����ȾĿ�꣨��Ļ��
			//renderer->SetRenderTarget(bitmap.Get());	//���õ�ǰ��ȾĿ��Ϊbitmap
			renderer->BindPipeline(&Renderer2D::render_shape2d_pipeline);  //ʹ��2D����λ�����Ⱦ����
			Renderer2D::FillRect(Utility::Rect{ 200, 200, 200, 200 }, Utility::Color{ 0.0f, 1.0f, 1.0f, 1.0f });
																		
			//renderer->BindPipeline(&Renderer2D::render_texture2d_pipeline);  //ʹ��2D��ͼ��Ⱦ����
			//renderer->SetDefaultTarget();								//
			//sprite->Render();											//���ƾ���
			
		});
		renderer->PreemptDisable();			//�����ռ����Ⱦ�̵߳��Լ���������Ȼ���������֡��������ɣ�
	});

	
}
