#ifndef __chjutils_h_20250123_
#define __chjutils_h_20250123_

#include <Ntsecapi.h> // for LSA_HANDLE
#include <Lm.h> // for NetApiBufferFree 

//////////////////////////////////////////////////////////////////////////


#include <EnsureClnup_mswin.h>

MakeCleanupPtrClass_winapi(Cec_LsaClose, NTSTATUS, LsaClose, LSA_HANDLE)
// -- for LsaOpenPolicy

MakeCleanupPtrClass_winapi(Cec_LsaFreeMemory, NTSTATUS, LsaFreeMemory, PVOID)
// -- for LsaLookupNames2

MakeCleanupPtrClass_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)
// -- for NetLocalGroupEnum etc

inline NTSTATUS _LsaFreeMemory(LSA_UNICODE_STRING *pus){ return LsaFreeMemory(pus); }
MakeCleanupPtrClass(Cec_LsaFreeMemory_UNICODE_STRING, NTSTATUS, _LsaFreeMemory, LSA_UNICODE_STRING*)
// -- LsaEnumerateAccountRights() returns an LSA_UNICODE_STRING[] that needs freeing by LsaFreeMemory. 

//////////////////////////////////////////////////////////////////////////

template<int nEle>
TCHAR* Combine_DomAndName(TCHAR (&outbuf)[nEle], const TCHAR *szDom, const TCHAR *szName)
{
	if(!szDom || !szDom[0])
		_sntprintf_s(outbuf, _TRUNCATE, _T("%s"), szName);
	else
		_sntprintf_s(outbuf, _TRUNCATE, _T("%s\\%s"), szDom, szName);
	return outbuf;
}

#include <JAutoBuf.h>

typedef JAutoBuf<TCHAR, sizeof(TCHAR), 1> AutoTCHARs;

inline bool Is_LessBuffer(DWORD winerr)
{
	if( winerr==ERROR_INSUFFICIENT_BUFFER ||
		winerr==ERROR_MORE_DATA ||
		winerr==ERROR_BUFFER_OVERFLOW)
		return true;
	else
		return false;
}

inline const TCHAR* sorf(BOOL succ)
{
	return succ ? _T("success") : _T("fail");
}

//////////////////////////////////////////////////////////////////////////

//// PSSA2000 specific:

const TCHAR *WinerrStr(DWORD winerr=0);
const TCHAR *app_WinErrStr(DWORD winerr); // same as above


//
// User-callbacks for CH10_DumpSD()
//
TCHAR* InterpretRights_Generic(DWORD rights, void *userctx);
TCHAR* InterpretRights_Token(DWORD rights, void *userctx);



//////////////////////////////////////////////////////////////////////////

#include <itc/InterpretConst.h>
#include <mswin/winuser.itc.h>
#include <mswin/commctrl.itc.h>
#include <mswin/winnt.itc.h>
#include <mswin/AclUI.itc.h>
#include <mswin/AccCtrl.itc.h>
#include <mswin/PrSht.itc.h>

using namespace itc;


#endif
