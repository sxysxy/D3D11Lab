#pragma once
#include "windows.h"
#include "stdio.h"

/*
	Ϊ�˽��һЩ���ӿ�ĳ�ͻ������fclose��vcruntime140-ruby��msvcrt�ж����ڣ����Ҳ�����ͨ�ã�ʹ��msvcrt��_wfopn����ʹ��
	msvcrt��fclose�����������У���������ģ�顣
				��ruby����������ʱ�򶼿������ˣ���ɶ����ƴ���֮��������ˡ�����

*/

namespace MSVCRT {

	extern int(*fclose) (FILE *fp);

	void GetFunctions();
}