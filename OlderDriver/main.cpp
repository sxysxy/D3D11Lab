#include "stdafx.h"
#include "client.h"
#include "renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow){
	AllocConsole();
	freopen("CON", "w", stdout);

    Client client(L"Demo", 600, 400);
    client.Initialize();
	client.renderer.frame_rate = 60;
    client.Mainloop([] (Renderer *renderer){
		Sleep(20);
	});
    return 0;
}