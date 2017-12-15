// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <fstream>
#include <wrl\client.h>
#include <string>
template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;
typedef std::basic_string<wchar_t> wstring;

// TODO: 在此处引用程序需要的其他头文件
