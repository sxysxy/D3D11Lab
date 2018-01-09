#include "HFWindow.h"
#include "extension.h"

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
		{
			HFWindow * w = (HFWindow *)GetWindowLong(hWnd, GWL_USERDATA);
			w->OnClosed();
		}
		return 0;
	case WM_CLOSE:
		{
			HFWindow * w = (HFWindow *)GetWindowLong(hWnd, GWL_USERDATA);
			w->OnClosed();
		}
		return 0;
	case WM_SIZE:
		{
			HFWindow * w = (HFWindow *)GetWindowLong(hWnd, GWL_USERDATA);
			w->OnResized();
			return 0;
		}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
}

void HFWindow::Resize(int w, int h) {
	assert(_native_handle);

	RECT r = { 0, 0, w, h };
	AdjustWindowRect(&r, wstyle, false);
	SetWindowPos(_native_handle, 0, 0, 0, r.right, r.bottom, SWP_NOMOVE);
}

void HFWindow::OnResized() {
	assert(_native_handle);

	RECT r;
	GetClientRect(_native_handle, &r);
	_width = r.right - r.left;
	_height = r.bottom - r.top;
}

void HFWindow::OnClosed() {
	PostQuitMessage(0);
}

void HFWindow::Create(const cstring &_title, int w, int h) {
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
		wc.lpszClassName = TEXT("23333");
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		RegisterClass(&wc);
		_native_inited = true;
	}

	RECT crect = { 0, 0, width, height };
	AdjustWindowRect(&crect, wstyle, false);
	int cw = crect.right - crect.left;
	int ch = crect.bottom - crect.top;
	_native_handle = CreateWindow(TEXT("23333"), title.c_str(), wstyle,
		(GetSystemMetrics(SM_CXSCREEN) - cw) >> 1,
		(GetSystemMetrics(SM_CYSCREEN) - ch) >> 1,
		cw, ch,
		0, 0, instance, this);
}

namespace Ext {
	namespace HFWindow{
		using namespace Utility;

		VALUE klass;

		void Delete(RHFWindow *wnd) {
			delete wnd;
		}

		VALUE New(VALUE klass) {
			RHFWindow *wrap = new RHFWindow;
			return Data_Wrap_Struct(klass, nullptr, Delete, wrap);
		}

		static VALUE initialize(VALUE self, VALUE title, VALUE w, VALUE h) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			std::wstring wtitle;
			U8ToU16(rb_string_value_cstr(&title), wtitle);
			wnd->Initialize(wtitle.c_str(), FIX2INT(w), FIX2INT(h));
			return self;
		}

		static VALUE show(VALUE self) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			wnd->Show();
			return self;
		}

		static VALUE hide(VALUE self) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			wnd->Hide();
			return self;
		}

		static VALUE native_handle(VALUE self) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			return INT2FIX(wnd->native_handle);
		}

		static VALUE set_title(VALUE self, VALUE title) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			std::wstring wtitle;
			U8ToU16(rb_string_value_cstr(&title), wtitle);
			wnd->SetTitle(wtitle);
			return title;
		}

		static VALUE resize(VALUE self, VALUE w, VALUE h) {
			auto *wnd = GetNativeObject<RHFWindow, ::HFWindow>(self);
			wnd->Resize(FIX2INT(w), FIX2INT(h));
			return self;
		}

		void Init() {
			klass = rb_define_class("HFWindow", rb_cObject);
			rb_define_alloc_func(klass, New);
			rb_define_method(klass, "initialize", (rubyfunc)initialize, 3);
			rb_define_method(klass, "show", (rubyfunc)show, 0);
			rb_define_method(klass, "hide", (rubyfunc)hide, 0);
			rb_define_method(klass, "native_handle", (rubyfunc)native_handle, 0);
			rb_define_method(klass, "set_title", (rubyfunc)set_title, 1);
			rb_define_method(klass, "resize", (rubyfunc)resize, 2);
			
		}
	}
}