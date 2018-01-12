#pragma once
#ifdef _MSC_VER
#pragma warning(disable:4005 4390)  
#endif

#include <Windows.h>
#include <d3d11.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>
#include <wingdi.h>
#include <ctype.h>
#include <algorithm>
#include <vector>
#include <functional>
#include <queue>
#include <DirectXMath.h>
#include <DirectXMathVector.inl>
#include <D3DX11.h>
#include <wrl/client.h>
#include <xinput.h>
#include <memory>
template<
    typename T
> using ComPtr = Microsoft::WRL::ComPtr<T>;
using namespace DirectX;

struct Color32 {
    union {
        struct {
            BYTE r;
            BYTE g;
            BYTE b;
            BYTE a;
        }rgba;
        DWORD color;
    };
    Color32(DWORD col = 0) {
        color = col;
    }
    Color32(BYTE _r, BYTE _g, BYTE _b, BYTE _a) {
        rgba.r = _r, rgba.g = _g, rgba.b = _b, rgba.a = _a;
    }
};

#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}