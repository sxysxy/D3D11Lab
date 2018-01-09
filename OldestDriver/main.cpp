#include "stdafx.h"
#include "extension.h"
#include "HFWindow.h"
#include <regex>

int __cdecl cmain(int argc, char *argv_[]) {
	ruby_sysinit(&argc, (char***)&argv_);  //这个ruby_sysinit一定要有，哪怕不用命令行参数。
	{
		RUBY_INIT_STACK;
		ruby_init();
		Ext::ApplyExtensions();
	
		VALUE script = rb_str_new_cstr("./main.rb");
		int state = 0;
		rb_load_protect(script, 0, &state);
		if (state) {
			rb_funcall(rb_mKernel, rb_intern("msgbox"), 1, rb_errinfo());
		}
		return state;
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow) {
	MSVCRT::GetFunctions();
	
	if (GetFileAttributes(TEXT("main.rb")) == INVALID_FILE_ATTRIBUTES) {
	MessageBox(0, TEXT("main.rb not found, stop."), TEXT("Error"), MB_OK);
	return 0;
	}

	return cmain(0, nullptr);
	

}