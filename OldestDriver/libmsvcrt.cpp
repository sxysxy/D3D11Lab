#include "libmsvcrt.h"

namespace MSVCRT {
	int(*fclose) (FILE *fp);

	void GetFunctions() {
		HMODULE h = GetModuleHandleW(L"msvcrt");

		fclose = (int(*)(FILE*))GetProcAddress(h, "fclose");
	}
}
