#if PlatformName==Win32
# include "Win32\component_p.c"
#elif PlatformName==x64
# include "x64\component_p.c"
#else
# error This PlatformName is not supported.
#endif
