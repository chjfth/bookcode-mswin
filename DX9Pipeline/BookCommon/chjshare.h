#pragma once

#include <commdefs.h>
#include <_MINMAX_.h>
#include <sdring.h>
#include <makeTsdring.h>

#include <EnsureClnup_mswin.h>
#include <mm_snprintf.h>

inline void ComPtr_Release(IUnknown *puk)
{ 
	puk->Release(); 
}
MakeDelega_CleanupPtr(Cec_Release, void, ComPtr_Release, IUnknown*);


inline int my_mm_vsnprintf(TCHAR *buf, size_t bufchars, const TCHAR *fmt, va_list args)
{
	return mm_vsnprintf(buf, (int)bufchars, fmt, args);
}
