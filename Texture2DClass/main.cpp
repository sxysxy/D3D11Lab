#include "stdafx.h"
#include "Client.h"
#include "global.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Sprite.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                        LPSTR lpCmd, int nCmd) {
    Client client(L"我就是叫紫妈怎么了alsdkjaeiovfakdlajsdl", 500, 500);
    client.Initialize();
    Texture2D texture(L"250px-Yukari.jpg");
    Sprite sprite(&texture);
    client.Mainloop([&](Renderer *renderer) {
        renderer->vsync = true; //垂直同步
        renderer->Clear();
        sprite.Render();
        renderer->Present();
    });
    return 0;
}