#pragma once

#include <vector>
#include <algorithm>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <ctype.h>
#include <cmath>
#include <functional>
#include <embeddedruby.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <wrl\client.h>
#include <referptr.h>
#include <fpstimer.h>
#include <shapes.h>
#include <errno.h>
#include <assert.h>
#include "libmsvcrt.h"
template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}

#ifdef _UNICODE
typedef std::wstring cstring;
#else 
typedef std::string cstring;
#endif

