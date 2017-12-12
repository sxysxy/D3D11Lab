#pragma once

#include "stdafx.h"
#include "global.h"
#include "Renderer.h"

class Client {
    HWND _hWnd;
public:
    wstring title;
    int width, height;
    HINSTANCE hInstance;
    const HWND &hWnd = _hWnd;

    BOOL quit;

    Client(const wstring &title_, int w, int h) {
        title.append(title_);
        width = w;
        height = h;
        quit = false;
    }
    ~Client();

    void Initialize();
    void Mainloop(const std::function<void(Renderer *renderer)> &callback);
private:

    void InitWindow();
    
   
};