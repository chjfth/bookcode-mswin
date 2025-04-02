#ifndef __chjutils_h_20250123_
#define __chjutils_h_20250123_

#include <Ntsecapi.h> // for LSA_HANDLE
#include <Lm.h> // for NetApiBufferFree 

#include <vaDbg.h>

typedef DWORD WinError_t;


//////////////////////////////////////////////////////////////////////////


#include <EnsureClnup_mswin.h>

MakeDelega_CleanupPtr_winapi(Cec_LsaClose, NTSTATUS, LsaClose, LSA_HANDLE)
// -- for LsaOpenPolicy

MakeDelega_CleanupPtr_winapi(Cec_LsaFreeMemory, NTSTATUS, LsaFreeMemory, PVOID)
// -- for LsaLookupNames2

MakeDelega_CleanupPtr_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)
// -- for NetLocalGroupEnum etc

inline NTSTATUS _LsaFreeMemory(LSA_UNICODE_STRING *pus){ return LsaFreeMemory(pus); }
MakeDelega_CleanupPtr_winapi(Cec_LsaFreeMemory_UNICODE_STRING, NTSTATUS, _LsaFreeMemory, LSA_UNICODE_STRING*)
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
		winerr==ERROR_BUFFER_OVERFLOW ||
		winerr==ERROR_BAD_LENGTH )
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

struct Cxx_SID_AND_ATTRIBUTES : public SID_AND_ATTRIBUTES
{
	Cxx_SID_AND_ATTRIBUTES(void *psid=nullptr)
	{
		Sid = psid;
		Attributes = 0;

//		vaDbgS(_T("Cxx_SID_AND_ATTRIBUTES ctor(%p)"), this);
	}

	~Cxx_SID_AND_ATTRIBUTES()
	{
		delete (char*)Sid;

//		vaDbgS(_T("Cxx_SID_AND_ATTRIBUTES dtor(%p) Sid=%p"), this, Sid); // ok
	}
};

MakeDelega_CleanupCxxPtr_en(Cxx_SID_AND_ATTRIBUTES, 
	Cec_SID_AND_ATTRIBUTES, CecArray_SID_AND_ATTRIBUTES)

MakeDelega_CleanupCxxPtr_en(LUID_AND_ATTRIBUTES, 
	Cec_LUID_AND_ATTRIBUTES, CecArray_LUID_AND_ATTRIBUTES)

WinError_t ab_GetSidFromAccountName(const TCHAR *accname, Jautobuf &abSid);


const TCHAR* format_wetime_as_localtime(LARGE_INTEGER twe, TCHAR output[], int osize);


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
