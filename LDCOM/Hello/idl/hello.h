#if defined(_M_IX86)
# include "Win32/hello.h"
#elif defined(_M_AMD64)
# include "x64/hello.h"
#else
# error Target platform not supported.
#endif
