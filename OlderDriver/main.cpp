#include "stdafx.h"
#include "client.h"
#include "renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){
    Client client(L"Demo", 600, 400);
    client.Initialize();
    client.Mainloop([] (Renderer *renderer){});
    return 0;
}