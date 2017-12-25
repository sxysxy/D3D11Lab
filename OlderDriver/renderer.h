#pragma once

#include "stdafx.h"

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
    long long timer_freq;
    HWND window;
    int _width, _height;

    void RenderImmediately();
    void RenderVsync();

public:
    const DWORD &phase = _phase;
    
    const int &width = _width, &height = _height;

    std::thread render_thread;

    int frame_rate; bool vsync;

    std::queue<RenderTask> tasks;

    Renderer();
    void Initialize(HWND hWnd);
    void Mainloop();
    void SetDefaultTarget();
    void Clear();
    void Resize(int w, int h);

    inline void Terminate() {
		_phase |= RENDERER_TERMINATED;
        render_thread.join();
    }


    //---D3D layer, Pretends to be private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swap_chain;
    ComPtr<ID3D11RenderTargetView> main_target;  //main(background) render target view;
    ID3D11RenderTargetView *current_target;      //current target.
};
