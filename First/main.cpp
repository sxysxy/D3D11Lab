#include "Common.h"
#include "D3D11Application.h"
#include <ctime>
wchar_t title[1000];
int WINAPI WinMain(HINSTANCE hInstance, 
            HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow) {
    srand(time(NULL));
    D3D11Application app;
    app.init(L"Demo", 600, 600);

    app.update = [&]() {
        D3D11Renderer &renderer = *app.renderer;
        
        UINT stride = sizeof(DirectX::XMFLOAT4)*1;
        UINT offset = 0;
        renderer.d3dimmContext->IASetVertexBuffers(0, 1, &renderer.vb, &stride, &offset);
        renderer.d3dimmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

		renderer.d3dimmContext->UpdateSubresource(renderer.cbuffer_ps.Get(), 0, 0, color, 0, 0);
        renderer.d3dimmContext->Draw(4, 0);
        
    };
    app.mainloop();
    return 0;
}