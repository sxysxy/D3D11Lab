#include "Common.h"
#include "D3D11Application.h"

int WINAPI WinMain(HINSTANCE hInstance, 
            HINSTANCE hPrevInstance, LPSTR lpCmd, int nCmdShow) {
    D3D11Application app;
    app.init(L"Demo", 600, 600);
    return 0;
}