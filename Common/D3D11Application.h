#pragma once

#include "Common.h"
#include "D3D11Renderer.h"

class D3D11Application {    
public:

    HINSTANCE hInstance;
    HWND hWnd;
    HDC hDC;

    D3D11Renderer *renderer;
    RECT rect;

    BOOL quit;

private:

    void initD3D11();

public:

    inline D3D11Application() {
        RtlZeroMemory(this, sizeof(D3D11Application));
    }

    ~D3D11Application();

    void init(wchar_t *title, int w, int h);
    void mainloop();
    void terminate();

    void render();
};