#include "Common.h"
#include "D3D11Application.h"
#include <ctime>
wchar_t title[1000];
int WINAPI WinMain(HINSTANCE hInstance, 
            HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow) {
    srand(time(NULL));
    D3D11Application app;
    app.init(L"Demo", 600, 600);

    
    long long freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    long long last;
    int counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&last);
    

    app.update = [&]() {
        D3D11Renderer &renderer = *app.renderer;
        
        UINT stride = sizeof(D3D11Renderer::VERTEX);
        UINT offset = 0;
        renderer.d3dimmContext->IASetVertexBuffers(0, 1, &renderer.vb, &stride, &offset);
        renderer.d3dimmContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        renderer.d3dimmContext->Draw(4, 0);

        counter++;
        long long now;
        QueryPerformanceCounter((LARGE_INTEGER*)&now);
        if (now - last > freq) {
            long long t = now-last-freq;
            last = now+t;
            wsprintfW(title, L"Demo - FPS : %d", counter);
            counter = 0;
            SetWindowText(app.hWnd, title);
        }
        
    };
    app.mainloop();
    return 0;
}