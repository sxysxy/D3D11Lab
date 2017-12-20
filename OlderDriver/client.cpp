#include "stdafx.h"
#include "Client.h"

void Client::Initialize() {
    hInstance = GetModuleHandle(0);
    InitWindow();
    renderer.Initialize(hWnd);
}
Client::~Client() {
    renderer.Terminate();
}
void Client::Mainloop(const std::function<void(Renderer *)> &callback) {
    MSG msg;
    while (!quit) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)quit = true;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            callback(&renderer);
        }
    }
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
    _hWnd = CreateWindow(L"23333", title.c_str(), wstyle, 220, 233, crect.right - crect.left, crect.bottom - crect.top,
        0, 0, hInstance, 0);
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);
}
