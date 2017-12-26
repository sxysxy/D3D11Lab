#include "stdafx.h"
#include "client.h"
#include "renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){
	//AllocConsole();
	//freopen("CON", "w", stdout);

    Client client(L"Demo", 600, 600);
    client.Initialize();
	client.renderer.SetFrameRate(60);
	client.SetFrameRate(60);
    client.Mainloop([] (Renderer *renderer){});
    return 0;
}