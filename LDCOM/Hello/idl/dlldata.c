#if defined(_M_IX86)
# include "gen32/dlldata.c"
#elif defined(_M_AMD64)
# include "gen64/dlldata.c"
#else
# error Target platform not supported.
#endif
