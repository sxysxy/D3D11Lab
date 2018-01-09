#pragma once
#include "stdafx.h"
#include "extension.h"

class HFWindow : public Utility::ReferredObject{
	cstring title;
	HWND _native_handle;
	int _width, _height;

	static const UINT wstyle = WS_OVERLAPPEDWINDOW;
	static bool _native_inited;

	static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 


public:

	const HWND &native_handle = _native_handle;
	const int &width = _width, &height = _height;

	HFWindow() {
		_native_handle = 0;
		_width = _height = 0;
	}
	HFWindow(const cstring &_title, int w, int h) :HFWindow() {
		Initialize(_title, w, h);
	}
	~HFWindow() {
		Uninitialize();
	}

	//this Initalize is for ruby extension, for RB_NEWOBJ_OF don't call the constructor function.
	template<typename ... _Arg>
	void Initialize(const _Arg & ..._arg) {
		_native_handle = 0;
		_width = _height = 0;
		Create(_arg ...);
	}
	//destructor.
	void Uninitialize() {
		title.~basic_string();
		if (_native_handle) {
			DestroyWindow(_native_handle);
			_native_handle = 0;
		}
	}
	void Release() {

	}


	void Create(const cstring &_title, int w, int h);

	cstring GetTitle() {
		return title;
	}
	void SetTitle(const cstring &t) {
		assert(_native_handle);

		title = t;
		SetWindowText(_native_handle, title.c_str());
	}
	void Show() {
		assert(_native_handle);

		ShowWindow(_native_handle, SW_NORMAL);
	}
	void Hide() {
		assert(_native_handle);

		ShowWindow(_native_handle, SW_HIDE);
	}
	void Resize(int w, int h);
	void OnResized();
	void OnClosed();
};

namespace Ext {
	namespace HFWindow{

		typedef RTemplate<::HFWindow> RHFWindow;

		extern VALUE klass;
		void Init();
		VALUE New(VALUE klass);
	}
}

