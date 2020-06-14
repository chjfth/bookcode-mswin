#if PlatformName==Win32
# include "Win32\dlldata.c"
#elif PlatformName==x64
# include "x64\dlldata.c"
#else
# error This PlatformName is not supported.
#endif
