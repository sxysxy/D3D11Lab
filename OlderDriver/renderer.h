#pragma once

#include "stdafx.h"

struct FPSCTRL {
	long long time;
	long long startt;
	double tick;
	int rate;
	void Restart(int r) {
		rate = r;

		long long freq;
		QueryPerformanceFrequency((PLARGE_INTEGER)&freq);
		QueryPerformanceCounter((PLARGE_INTEGER)&startt);
		tick = 1.0 * freq / rate;
		time = 0;
	}

	void Await() {
		auto ticktime = tick * ++time;
		while (true) {
			long long nowt;
			QueryPerformanceCounter((PLARGE_INTEGER)&nowt);
			long long d = nowt - startt;
			if (d >= ticktime)break;
			if (d >= 20000)Sleep(1);
			else
				std::this_thread::yield();
		}
	}
};

class Renderer;

struct RenderTask {
    std::function<void(Renderer *renderer)> call;
};

const DWORD RENDERER_PREPARING = 1;
const DWORD RENDERER_READY = 2;
const DWORD RENDERER_RENDERING = 4;
const DWORD RENDERER_TERMINATED = 8;

class Renderer {
    DWORD _phase;
    HWND window;
    int _width, _height;

    void RenderImmediately();
    void RenderVsync();
	int _frame_rate;
	FPSCTRL *fpsctrl;

public:
    const DWORD &phase = _phase;
    
    const int &width = _width, &height = _height;
	const int &frame_rate = _frame_rate;

    std::thread render_thread;

    bool vsync;

    std::queue<RenderTask> tasks;

    Renderer();
	~Renderer();
    void Initialize(HWND hWnd);
    void Mainloop();
    void SetDefaultTarget();
    void Clear();
    void Resize(int w, int h);

    inline void Terminate() {
		_phase |= RENDERER_TERMINATED;
        render_thread.join();
    }
	void SetFrameRate(int f);


    //---D3D layer, Pretends to be private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swap_chain;
    ComPtr<ID3D11RenderTargetView> main_target;  //main(background) render target view;
    ID3D11RenderTargetView *current_target;      //current target.
};
