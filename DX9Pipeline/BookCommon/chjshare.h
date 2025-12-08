#pragma once

#include <commdefs.h>
#include <_MINMAX_.h>
#include <sdring.h>
#include <makeTsdring.h>

#include <EnsureClnup_mswin.h>
#include <mm_snprintf.h>

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>

#include <InterpretConst.h>

using namespace itc;
#include <DirectX\d3d9.itc.h>
#include <DirectX\d3dx9.itc.h>
#include <DirectX\directx_errors.itc.h>


inline void ComPtr_Release(IUnknown *puk)
{ 
	puk->Release(); 
}
MakeDelega_CleanupPtr(Cec_Release, void, ComPtr_Release, IUnknown*);


inline int my_mm_vsnprintf(TCHAR *buf, size_t bufchars, const TCHAR *fmt, va_list args)
{
	return mm_vsnprintf(buf, (int)bufchars, fmt, args);
}
