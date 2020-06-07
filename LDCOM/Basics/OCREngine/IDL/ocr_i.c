#if defined(_M_IX86)
# include "Win32\ocr_i.c"
#elif defined(_M_AMD64)
# include "x64\ocr_i.c"
#else
# error Target platform not supported.
#endif
