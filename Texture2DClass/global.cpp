#include "global.h"

Renderer g_renderer;
HWND g_hMainwindow;

static HMODULE hntdll = LoadLibrary(L"ntdll.dll");
pNtDelayExecution NtDelayExecution = (pNtDelayExecution)GetProcAddress(hntdll, "NtDelayExecution");
