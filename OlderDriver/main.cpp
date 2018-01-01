#include "stdafx.h"
#include "client.h"
#include "renderer.h"
#include "shaders.h"
#include "bitmap.h"
#include "sprite.h"
using namespace Utility;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){

    Client client(L"��û���Ҿ��ǽ�����qjwajdalkjdkdas", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(233);  //��Ⱦ�߳�֡��
	client.SetFrameRate(60);		   //�߼��߳�֡��
	Renderer2D::CreatePipelines();     //����2D��Ⱦ����ĳ�����Ⱦ����

	float positions[4][2] = {
		{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}
	};
	float colors[4][4] = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },   //red
		{ 0.0f, 1.0f, 0.0f, 1.0f },   //green
		{ 0.0f, 0.0f, 1.0f, 1.0f },   //blue
		{ 1.0f, 0.0f, 1.0f, 1.0f }    //purple
	};
	auto bitmap = ReferPtr<Bitmap>::New(L"../CommonFiles/250px-Yukari.jpg");   //�����̵���������ָ��
	auto sprite = ReferPtr<Sprite>::New(bitmap.Get());
	sprite->x = sprite->y = 175;
	client.Mainloop([&](Renderer *renderer) {
		renderer->Preempt();				//��ռ��ǰ��ͼ֡
		renderer->PushTask([&](Renderer *renderer) {
			renderer->SetDefaultTarget();		//���õ�ǰ��Ⱦģ��ΪĬ��Ŀ�꣨����ʾ���ϵĴ��ڣ�
			renderer->Clear();					//�����ȾĿ�꣨��Ļ��
			renderer->SetRenderTarget(bitmap.Get());	//���õ�ǰ��ȾĿ��Ϊbitmap
			renderer->BindPipeline(&Renderer2D::render_shape2d_pipeline);  //ʹ��2D����λ�����Ⱦ����
			Renderer2D::DrawRect(renderer, positions, colors);			//�����水������������Ż���һ������
																		//֮�����ɫ�������Բ�ֵ�����Բ�
			renderer->BindPipeline(&Renderer2D::render_texture2d_pipeline);  //ʹ��2D��ͼ��Ⱦ����
			renderer->SetDefaultTarget();								//
			sprite->Render();											//���ƾ���
			
		});
		renderer->PreemptDisable();			//�����ռ����Ⱦ�̵߳��Լ���������Ȼ���������֡��������ɣ�
	});

    return 0;
}