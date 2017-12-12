#pragma once

#include "stdafx.h"
#include "Client.h"
#include "Renderer.h"

extern Renderer g_renderer;
extern HWND g_hMainwindow;

typedef NTSTATUS(__stdcall * pNtDelayExecution) (BOOL, PLARGE_INTEGER);
extern pNtDelayExecution NtDelayExecution;