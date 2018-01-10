#pragma once

#ifdef _WIN64

#include "C:/ruby250-x64-mswin64/include/ruby-2.5.0/ruby.h"
#pragma comment(lib, "C:/ruby250-x64-mswin64/lib/vcruntime140-ruby250.lib")

#else

#include "C:/ruby250-mswin32/include/ruby-2.5.0/ruby.h"
#pragma comment(lib, "C:/ruby250-mswin32/lib/vcruntime140-ruby250.lib")
//fclose is not only in msvcrt, but also in vcruntime140-ruby250...

#endif


