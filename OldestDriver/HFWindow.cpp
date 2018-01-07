#include "HFWindow.h"

bool HFWindow::_native_inited = false;

LRESULT CALLBACK HFWindow::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CREATE:
		{
			CREATESTRUCT * pc = (CREATESTRUCT *)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)pc->lpCreateParams);
			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		{
			HFWindow * w = (HFWindow *)GetWindowLong(hWnd, GWL_USERDATA);
			w->OnResize();
			return 0;
		}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
}

void HFWindow::OnResize() {
	assert(_native_handle);

	RECT r;
	GetClientRect(_native_handle, &r);
	_width = r.right - r.left;
	_height = r.bottom - r.top;
}

void HFWindow::Create(const std::wstring &_title, int w, int h) {
	_width = w, _height = h;
	title = _title;
	HINSTANCE instance = GetModuleHandle(0);

	if(!_native_inited){
		WNDCLASS wc;
		RtlZeroMemory(&wc, sizeof(wc));
		wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
		wc.hCursor = LoadCursor(instance, IDC_ARROW);
		wc.hIcon = LoadCursor(instance, IDI_WINLOGO);
		wc.hInstance = instance;
		wc.lpfnWndProc = HFWindow::_WndProc;
		wc.lpszClassName = L"23333";
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		RegisterClass(&wc);
		_native_inited = true;
	}

	RECT crect = { 0, 0, width, height };
	AdjustWindowRect(&crect, wstyle, false);
	int cw = crect.right - crect.left;
	int ch = crect.bottom - crect.top;
	_native_handle = CreateWindow(L"23333", title.c_str(), wstyle,
		(GetSystemMetrics(SM_CXSCREEN) - cw) >> 1,
		(GetSystemMetrics(SM_CYSCREEN) - ch) >> 1,
		cw, ch,
		0, 0, instance, this);
}