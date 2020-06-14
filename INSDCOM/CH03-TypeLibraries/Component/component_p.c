#if defined(_M_IX86)
# include "Win32\component_p.c"
#elif defined(_M_AMD64)
# include "x64\component_p.c"
#else
# error Target platform not supported.
#endif
