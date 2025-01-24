#ifndef __chjutils_h_20250123_
#define __chjutils_h_20250123_

#include <EnsureClnup_mswin.h>

#include <Ntsecapi.h> // for LSA_HANDLE
#include <Lm.h> // for NetApiBufferFree 


MakeCleanupPtrClass_winapi(Cec_LsaClose, NTSTATUS, LsaClose, LSA_HANDLE)
// -- for LsaOpenPolicy

MakeCleanupPtrClass_winapi(Cec_LsaFreeMemory, NTSTATUS, LsaFreeMemory, PVOID)
// -- for LsaLookupNames2

MakeCleanupPtrClass_winapi(Cec_LocalFree, HLOCAL, LocalFree, HLOCAL)
// -- for ConvertSidToStringSid

MakeCleanupPtrClass_winapi(Cec_FreeSid, PVOID, FreeSid, PSID)
// -- for AllocateAndInitializeSid

MakeCleanupPtrClass_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)
// -- for NetLocalGroupEnum etc

inline NTSTATUS _LsaFreeMemory(LSA_UNICODE_STRING *pus){ return LsaFreeMemory(pus); }
MakeCleanupPtrClass(Cec_LsaFreeMemory_UNICODE_STRING, NTSTATUS, _LsaFreeMemory, LSA_UNICODE_STRING*)
// -- LsaEnumerateAccountRights() returns an LSA_UNICODE_STRING[] that needs freeing by LsaFreeMemory. 


//// PSSA2000 specific:

const TCHAR *WinerrStr(DWORD winerr=0);
const TCHAR *app_WinErrStr(DWORD winerr); // same as above



#define PSIDFromPACE(pACE) ((PSID)(&((pACE)->SidStart))) // CH10.2

TCHAR * SID2Repr(PSID pvSid, TCHAR buf[], int buflen);

void CH10_DumpACL( PACL pACL );
void CH10_DumpSD( PSECURITY_DESCRIPTOR pvsd );

//////////////////////////////////////////////////////////////////////////

#include "InterpretConst.h"

extern CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uMsg;
extern CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uPage;
extern CInterpretConst itc_SECURITY_INFORMATION;
extern CInterpretConst itc_SE_OBJECT_TYPE;
extern CInterpretConst itc_ACE_TYPE;
extern CInterpretConst itc_ACE_FLAGS;
extern CInterpretConst itc_SECURITY_DESCRIPTOR_CONTROL;
extern CInterpretConst itc_SI_ACCESS_flags;
extern CInterpretConst itc_SI_OBJECT_INFO_flags;
extern CInterpretConst itc_SI_INHERIT_TYPE_flags;
extern CInterpretConst itc_SERVICE_CONTROL;
extern CInterpretConst itc_DBT;



#endif
