#if defined(_M_IX86)
# include "Win32\ocr.h"
#elif defined(_M_AMD64)
# include "x64\ocr.h"
#else
# error Target platform not supported.
#endif
