#pragma once
#ifdef _MSC_VER
#pragma warning(disable:4005 4390)  
#endif

#include <mutex>
#include <functional>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <thread>
#include <deque>
#include <string>
#include "../Utility/embeddedruby.h" //MUST be included BEFORE '#include <windows.h>' and AFTER C++STL.
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <cstdarg>
#include <cctype>
#include <string>
#include <cstring>
#include <cmath>
#include <DirectXMath.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <wrl/client.h>
#include <mmsystem.h>

#include "../Utility/ReferPtr.h"
#include "../Utility/Shapes.h"

template<typename T>using ComPtr = Microsoft::WRL::ComPtr<T>;

#define RCHECK(x, msg) if(!(x)){MessageBoxW(0, msg, L"Error", 0); exit(0);}

namespace std {
    typedef basic_string<wchar_t> wstring;
}


