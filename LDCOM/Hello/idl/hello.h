#if defined(_M_IX86)
# include "gen32/hello.h"
#elif defined(_M_AMD64)
# include "gen64/hello.h"
#else
# error Target platform not supported.
#endif
