#pragma once

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <cstdarg>
#include <cctype>
#include <string>
#include <cstring>
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <cmath>
#include <DirectXMath.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <wrl/client.h>
#include <mutex>
#include <functional>
#include <algorithm>
#include <set>
#include <map>
#include <deque>
#include <wincodec.h>
#include <mmsystem.h>

#include "../Utility/ReferPtr.h"
#include "../Utility/Shapes.h"

template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;

#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}

namespace std {
    typedef basic_string<wchar_t> wstring;
}


