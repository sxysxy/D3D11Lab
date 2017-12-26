#pragma once

#include "stdafx.h"
#include "renderer.h"

class Client {
    HWND _hWnd;
public:
    std::wstring title;
    int width, height;
    HINSTANCE hInstance;
    const HWND &hWnd = _hWnd;
    Renderer renderer;

	FPSCTRL *fpsctrl;

    BOOL quit;

    Client(const std::wstring &title_, int w, int h) {
        title.append(title_);
        width = w;
        height = h;
        quit = false;
		fpsctrl = new FPSCTRL;
    }
    ~Client();

    void Initialize();
    void Mainloop(const std::function<void(Renderer *renderer)> &callback);
	void SetFrameRate(int r) {
		fpsctrl->Restart(r);
	}
private:

    void InitWindow();


};