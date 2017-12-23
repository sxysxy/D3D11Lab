// stdafx.cpp : 只包括标准包含文件的源文件
// ShaderCompiler.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "d3dx11d_x64.lib")
#else
#pragma comment(lib, "d3dx11_x64.lib")
#endif

#else    //x86

#ifdef _DEBUG
#pragma comment(lib, "d3dx11d_x86.lib")
#else
#pragma comment(lib, "d3dx11_x86.lib")
#endif

#endif