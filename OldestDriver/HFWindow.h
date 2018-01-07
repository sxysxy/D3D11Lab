#pragma once
#include "stdafx.h"

class HFWindow : public Utility::ReferredObject{
	std::wstring title;
	HWND _native_handle;
	int _width, _height;

	static const UINT wstyle = WS_OVERLAPPEDWINDOW;
	static bool _native_inited;

	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
	void OnResize();

public:
	const HWND &native_handle = _native_handle;
	const int &width = _width, &height = _height;

	HFWindow() {
		_native_handle = 0;
		_width = _height = 0;
	}
	HFWindow(const std::wstring &_title, int w, int h) :HFWindow() {
		Create(_title, w, h);
	}
	void Create(const std::wstring &_title, int w, int h);
	~HFWindow() {
		if (_native_handle)return;
		DestroyWindow(_native_handle);
	}

	std::wstring GetTitle() {
		return title;
	}
	void SetTitle(const std::wstring &t) {
		assert(_native_handle);

		title = t;
		SetWindowTextW(_native_handle, title.c_str());
	}
	void Show() {
		assert(_native_handle);

		ShowWindow(_native_handle, SW_NORMAL);
	}
	void Hide() {
		assert(_native_handle);

		ShowWindow(_native_handle, SW_HIDE);
	}
	void Resize(int w, int h) {
		assert(_native_handle);

		RECT r = { 0, 0, w, h };
		AdjustWindowRect(&r, wstyle, false);
		SetWindowPos(_native_handle, 0, 0, 0, r.right, r.bottom, SWP_NOMOVE);
		OnResize();
	}

};

