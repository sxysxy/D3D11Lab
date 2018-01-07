#include "stdafx.h"
#include "extension.h"
#include "HFWindow.h"

int __cdecl cmain(int argc, char *argv_[]) {
	ruby_sysinit(&argc, (char***)&argv_);  //这个ruby_sysinit一定要有，哪怕不用命令行参数。
	{
		RUBY_INIT_STACK;
		ruby_init();
		char *argv[2];  //
		argv[0] = new char[10];
		argv[1] = new char[10];
		strcpy(argv[0], "null");
		strcpy(argv[1], "main.rb");
		int ret = ruby_run_node(ruby_options(2, argv));   //第二个参数是char**的
		delete argv[0];
		delete argv[1];
		return ret;
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmd, int nShow) {
	MSVCRT::GetFunctions();

	/*
	if (GetFileAttributes(L"main.rb") == INVALID_FILE_ATTRIBUTES) {
	MessageBox(0, L"main.rb not found, stop.", L"Error", MB_OK);
	return 0;
	}

	return cmain(0, nullptr);
	*/

	auto window = Utility::ReferPtr<HFWindow>::New(L"Test", 600, 600);
	window->Show();
	Sleep(10000);
	return 0;
}