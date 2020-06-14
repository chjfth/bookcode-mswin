#if PlatformName==Win32
# include "Win32\component.h"
#elif PlatformName==x64
# include "x64\component.h"
#else
# error Target platform not supported.
#endif
