#if defined(_M_IX86)
# include "Win32\component_i.c"
#elif defined(_M_AMD64)
# include "x64\component_i.c"
#else
# error No suitable _M_XXX target platform macro detected.
#endif
