#include "stdafx.h"
#include "Client.h"
#include "global.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                        LPSTR lpCmd, int nCmd) {
    Client client(L"我就是叫紫妈怎么了alsdkjaeiovfakdlajsdl", 500, 500);
    client.Initialize();
    g_hMainwindow = client.hWnd;
    client.Mainloop();
    return 0;
}