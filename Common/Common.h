#pragma once

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
#include <queue>
#include <DirectXMath.h>
using namespace DirectX;

inline void releaseCom(IUnknown **com) {
   if(*com)(*com)->Release();
   *com = nullptr;
}

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