#include "stdafx.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "d3dx11d_x86.lib")
#else
#pragma comment(lib, "d3dx11_x86.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "d3dx11d_x64.lib")
#else
#pragma comment(lib, "d3dx11_x64.lib")
#endif
#endif