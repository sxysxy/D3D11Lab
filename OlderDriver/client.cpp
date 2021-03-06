#include "stdafx.h"
#include "Client.h"
#include "global.h"

void Client::Initialize() {
    hInstance = GetModuleHandle(0);
    InitWindow();
    renderer.Initialize(hWnd);
}
Client::~Client() {
	delete fpsctrl;
}

void Client::LogicLoop(const std::function<void(Renderer *)> &callback) {
	while (!quit) {
		callback(&renderer);
		fpsctrl->Await();
	}
}

void Client::MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Client::Mainloop(const std::function<void(Renderer *)> &callback) {

	std::thread r([&]() {renderer.MainloopProc();  });
	std::thread f([&]() {LogicLoop(callback); });
	MessageLoop();
	renderer.Terminate();
	quit = true;
	f.join();
	r.join();
}

void Client::InitWindow() {
    WNDCLASS wc;
    RtlZeroMemory(&wc, sizeof(wc));
    wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = LoadCursor(hInstance, IDI_WINLOGO);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (uMsg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
    };
    wc.lpszClassName = L"23333";
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    RegisterClass(&wc);
    UINT wstyle = WS_OVERLAPPEDWINDOW &~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    RECT crect = { 0, 0, width, height };
    AdjustWindowRect(&crect, wstyle, false);
	
	int cw = crect.right - crect.left;
	int ch = crect.bottom - crect.top;
    _hWnd = CreateWindow(L"23333", title.c_str(), wstyle, 
		(GetSystemMetrics(SM_CXSCREEN) - cw) >> 1, 
		(GetSystemMetrics(SM_CYSCREEN) - ch) >> 1, 
	    cw, ch,
        0, 0, hInstance, 0);
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
}
