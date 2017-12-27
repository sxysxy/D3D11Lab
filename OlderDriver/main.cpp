#include "stdafx.h"
#include "client.h"
#include "renderer.h"
#include "shaders.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){

    Client client(L"Demo", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(30);
	client.SetFrameRate(60);

	//Render Pipeline
	Renderer2D::CreatePipelines();
	client.renderer.BindPipeline(&Renderer2D::render_shape2d_pipeline);  //Using 2DShape RenderPipeline

	float positions[4][2] = {
		{-0.5f, -0.5f}, {-0.5f, 0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}
	};
	float colors[4][4] = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },   //red
		{ 0.0f, 1.0f, 0.0f, 1.0f },   //green
		{ 0.0f, 0.0f, 1.0f, 1.0f },   //blue
		{ 1.0f, 0.0f, 1.0f, 1.0f }    //purple
	};

	client.Mainloop([&](Renderer *renderer) {
		renderer->PushTask([&](Renderer *renderer) {Renderer2D::DrawRect(renderer, positions, colors); });

	});

    return 0;
}