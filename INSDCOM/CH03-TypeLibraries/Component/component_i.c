#if PlatformName==Win32
# include "Win32\component_i.c"
#elif PlatformName==x64
# include "x64\component_i.c"
#else
# error This PlatformName is not supported.
#endif
