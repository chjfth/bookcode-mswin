#if defined(_M_IX86)
# include "gen32/hello_p.c"
#elif defined(_M_AMD64)
# include "gen64/hello_p.c"
#else
# error Target platform not supported.
#endif
