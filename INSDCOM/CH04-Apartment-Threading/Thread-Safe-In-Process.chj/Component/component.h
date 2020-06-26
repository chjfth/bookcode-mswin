#if defined(_M_IX86)
# include "Win32\component.h"
#elif defined(_M_AMD64)
# include "x64\component.h"
#else
# error No suitable _M_XXX target platform macro detected.
#endif
