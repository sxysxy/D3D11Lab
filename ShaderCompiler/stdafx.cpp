// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ShaderCompiler.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

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
