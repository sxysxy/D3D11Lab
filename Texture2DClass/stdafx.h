#pragma once

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <string.h>
#include <string>
#include <cctype>
#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <thread>
#include <chrono>
template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;
#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}
using wstring = std::basic_string<wchar_t>;