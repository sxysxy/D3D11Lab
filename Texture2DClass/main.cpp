#include "stdafx.h"
#include "Client.h"
#include "global.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                        LPSTR lpCmd, int nCmd) {
    Client client(L"�Ҿ��ǽ�������ô��alsdkjaeiovfakdlajsdl", 500, 500);
    client.Initialize();
    g_hMainwindow = client.hWnd;
    client.Mainloop();
    return 0;
}