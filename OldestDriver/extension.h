#pragma once

/*extension for ruby lib*/
#include "stdafx.h"

namespace Ext {
	typedef VALUE(*rubyfunc)(...);
	void ApplyExtensions();

	static void U8ToU16(const char *s, std::wstring &out,UINT cp = CP_UTF8) {
		int len = MultiByteToWideChar(cp, 0, s, strlen(s), NULL, 0);
		wchar_t *t = new wchar_t[len + 1];
		MultiByteToWideChar(cp, 0, s, strlen(s), t, len);
		t[len] = '\0';
		out = t;
		delete t;
	}

	static void U16ToU8(const wchar_t *ws, std::string &out, UINT cp) {
		int len = WideCharToMultiByte(cp, 0, ws, wcslen(ws), NULL, NULL, NULL, NULL);
		char *t = new char[len + 1];
		WideCharToMultiByte(cp, 0, ws, lstrlenW(ws), t, len, NULL, NULL);
		t[len] = '\0';
		out = t;
		delete t;
	}

    template<class T>
    T *GetNativeObject(VALUE self) {
        T *obj;
        Data_Get_Struct(self, T, obj);
        return obj;
    }

	namespace DX {
		extern VALUE module;
		
	}
}