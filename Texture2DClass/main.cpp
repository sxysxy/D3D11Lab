#include "stdafx.h"
#include "Client.h"
#include "global.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Sprite.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                        LPSTR lpCmd, int nCmd) {
    Client client(L"�Ҿ��ǽ�������ô��alsdkjaeiovfakdlajsdl", 500, 500);
    client.Initialize();
    Texture2D texture(L"250px-Yukari.jpg");
    Sprite sprite(&texture);
    client.Mainloop([&](Renderer *renderer) {
        renderer->vsync = true; //��ֱͬ��
        renderer->Clear();
        sprite.Render();
        renderer->Present();
    });
    return 0;
}