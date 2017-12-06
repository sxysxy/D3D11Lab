#include "D3D11Application.h"

D3D11Application::~D3D11Application() {
    delete renderer;
}

void D3D11Application::init(wchar_t *title, int w, int h) {
    hInstance = GetModuleHandle(0);

    WNDCLASS wc;
    RtlZeroMemory(&wc, sizeof(WNDCLASS));
    wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
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
    wc.lpszClassName = _T("D3D11Application");
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    RCHECK(RegisterClass(&wc) != 0, _T("×¢²á´°¿ÚÀàÊ§°Ü"));

    rect.right = w, rect.bottom = h;
    UINT wstyle = WS_OVERLAPPEDWINDOW &~ (WS_THICKFRAME | WS_MAXIMIZEBOX);
    RECT r = rect;
    AdjustWindowRect(&r, wstyle, false);

    hWnd = CreateWindow(_T("D3D11Application"), title, wstyle, CW_USEDEFAULT, CW_USEDEFAULT,
                            r.right-r.left, r.bottom-r.top, 0, 0, 
                            hInstance, 0);
    ShowWindow(hWnd, SW_NORMAL);
    UpdateWindow(hWnd);

    initD3D11();
}

void D3D11Application::initD3D11() {
    renderer = new D3D11Renderer();
    renderer -> init(hWnd);
}

FLOAT backColor[] = { 0.0f, 1.0f, 0.0f, 0.0f };

void D3D11Application::render() {
    renderer->clear();
    if(update)update();
    renderer->present(0);
}

void D3D11Application::mainloop() {
    MSG msg;
    while (!quit) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)quit = true;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            render();
        }
    }
}

void D3D11Application::terminate() {
    quit = true;
}
