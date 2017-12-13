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
    bool flag = false;
    client.Mainloop([&](Renderer *renderer) {
        renderer->vsync = true; //垂直同步
        renderer->Clear();
        sprite.Render();
        renderer->Present();

        //update data:
        if(!flag && sprite.x < 250){
            sprite.x += 1;
            sprite.y += 1;
            if(sprite.x == 250)flag = true;
        }
        else {
            sprite.x = 0;
            sprite.y = 0;
            sprite.zoomx = sprite.zoomy = 2.0f;
        }
    });
    return 0;
}
