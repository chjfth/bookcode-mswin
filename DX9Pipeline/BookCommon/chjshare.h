#pragma once

#include <sdring.h>
#include <makeTsdring.h>

#include <EnsureClnup_mswin.h>

inline void ComPtr_Release(IUnknown *puk)
{ 
	puk->Release(); 
}
MakeDelega_CleanupPtr(Cec_Release, void, ComPtr_Release, IUnknown*);
